#pragma once
#include "Combat/JSCombatComponent.h"

#include "JSCharacter.h"
#include "Combat/JSCombatInterface.h"
#include "Components/BoxComponent.h"
#include "Sound/SoundCue.h"
#include "PaperZDCharacter.h"
#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

UJSCombatComponent::UJSCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UJSCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	
	// Find the attack hit box - UBoxComponent
	AActor* MyOwner = GetOwner();
	TSet<UActorComponent*> AllComponents = MyOwner->GetComponents();
	for (UActorComponent* ComponentPtr : AllComponents)
	{
		if (ComponentPtr == nullptr)
		{
			continue;	
		}
		
		UBoxComponent* BoxComponent = Cast<UBoxComponent>(ComponentPtr);
		if (BoxComponent == nullptr)
		{
			continue;
		}

		if (BoxComponent->GetName() == AttackHitboxName)
		{
			AttackHitBoxPtr = BoxComponent;
			FoundAttackHitBox++;
		}
	}

	// TODO Log error?
	//if (FoundAttackHitBox>1)
	
	// Bind on overlap
	UBoxComponent* HitBox = AttackHitBoxPtr.Get();
	if (HitBox != nullptr)
	{
		HitBox->OnComponentBeginOverlap.AddDynamic(this, &UJSCombatComponent::BeginAttackHitboxOverlap);
		HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Bind attack animation complete - We aren't using this method right now for our shoot
	//AttackAnimationOverideDelegate.BindUObject(this, &UJSCombatComponent::AttackAnimationComplete);

	// Save weakptr to AnimInstance
	if (const APaperZDCharacter* MyOwnerZD = Cast<APaperZDCharacter>(MyOwner))
	{
		AnimInstancePtr = MyOwnerZD->GetAnimInstance();	
	}

	// Save off relative location for shaking animation
	if (const APaperZDCharacter* Owner = Cast<APaperZDCharacter>(GetOwner()))
	{
		if (const UPaperFlipbookComponent* MySprite = Owner->GetSprite())
		{
			SpriteDefaultRelativeLocation = MySprite->GetRelativeLocation();
		}
	}

	// Bind to OnTakeAnyDamage
	MyOwner->OnTakeAnyDamage.AddDynamic(this, &UJSCombatComponent::OnTakeDamage);
}

void UJSCombatComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	//AttackAnimationOverideDelegate.Unbind();
}

void UJSCombatComponent::ToggleAttackHitbox(bool Enabled)
{
	UBoxComponent* HitBox = AttackHitBoxPtr.Get();
	if (HitBox != nullptr)
	{
		if (Enabled)
		{
			HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		else
		{
			HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void UJSCombatComponent::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	TakeDamage(Damage);
}

void UJSCombatComponent::TakeDamage(uint8 InDamage)
{
	CurrentHealth -= InDamage;
	CurrentHealth = FMath::Clamp(CurrentHealth, 0, MaxHealth);

	PlayHitStunSound();

	// Interrupt any existing attack overrides to cancel our outgoing attacks
	if (UPaperZDAnimInstance* MyAnimInstance = AnimInstancePtr.Get())
	{
		MyAnimInstance->StopAllAnimationOverrides();
	}

	if (CurrentHealth <= 0)
	{
		OnCombatActorDefeated.Broadcast();
		IsDead = true;
		
		if (UPaperZDAnimInstance* AnimInstance = AnimInstancePtr.Get())
		{
			AnimInstance->JumpToNode(ABPJumpName_Dead);
		}

		// TODO This should prob be handled by the character?
		if (const APaperZDCharacter* MyOwner = Cast<APaperZDCharacter>(GetOwner()))
		{
			if (UCapsuleComponent* Capsule = MyOwner->GetCapsuleComponent())
			{
				Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
	else
	{
		BeginHitStun();
		BeginHitPause();
		if (AJSCharacter* MyOwner = Cast<AJSCharacter>(GetOwner()))
		{
			MyOwner->DamageFlash();
		}
	}
}

bool UJSCombatComponent::CanAct() const
{
	return !IsStunned && !IsAttacking && !IsDead;
}

bool UJSCombatComponent::SpendShotEnergy(float ShotCost)
{
	if (ShotEnergy - ShotCost >= 0.0)
	{
		ShotEnergy -= ShotCost;
		DoAttack();
		return true;
	}
	return false;
}

void UJSCombatComponent::DoAttack()
{
	if (!CanAct())
	{
		return;
	}
	
	// Since we have a lot of different animation states, we aren't using the AttackAnim Sequence and slot paradigm
	// Instead DoAttack and AttackAnimationComplete are going to be called by an ANS on each attack animation in the Animation Source
	/*
	if (UPaperZDAnimInstance* AnimInstance = AnimInstancePtr.Get())
	{
		AnimInstance->PlayAnimationOverride(AttackAnimSequence, AttackAnimSequenceSlot, 1, 0, AttackAnimationOverideDelegate);
		IsAttacking = true;
	}
	*/
	
	// Instead we use a timer for shot animation length
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			ShootAnimationTimerHandle,
			this,
			&UJSCombatComponent::AttackAnimationComplete,
			ShootAnimationDuration,
			false);
		
		IsAttacking = true;
	}
}

void UJSCombatComponent::AttackAnimationComplete()
{
	// If (success) -> Completed, else Canceled.
	IsAttacking = false;
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ShootAnimationTimerHandle);
	}
	OnAttackEndSignature.ExecuteIfBound(true);
}

void UJSCombatComponent::RefundShotEnergy(float ShotCost)
{
	ShotEnergy = FMath::Clamp(ShotEnergy, ShotEnergy+ShotCost, MaxShotEnergy);	
}

void UJSCombatComponent::BeginAttackHitboxOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* Other,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (IJSCombatInterface* OtherCombatInterface = Cast<IJSCombatInterface>(Other))
	{
		if (UJSCombatComponent* OtherCombatComponent  = OtherCombatInterface->GetCombatComponent())
		{
			if (MyFaction != OtherCombatComponent->MyFaction)
			{
				// Do damage to the other combat component.
				OtherCombatComponent->TakeDamage(AttackDamage);

				BeginHitPause();
			}
		}
	}
}

void UJSCombatComponent::BeginHitStun()
{
	if (const UWorld* World = GetWorld())
	{
		FTimerHandle StunTimerHandle; // TODO Maybe keep this ref in .h?
		World->GetTimerManager().SetTimer(
			StunTimerHandle,
			this,
			&UJSCombatComponent::EndHitStun,
			StunDuration,
			false);
		
		IsStunned = true;
		OnStunStatusChanged.Broadcast(IsStunned);

		// Begin Stun Animation Timer
		World->GetTimerManager().SetTimer(
			StunAnimationTimerHandle,
			this,
			&UJSCombatComponent::StunAnimation,
			StunAnimationLoop,
			true);
	}
	
	if (UPaperZDAnimInstance* AnimInstance = AnimInstancePtr.Get())
	{
		AnimInstance->JumpToNode(ABPJumpName_HitStun);
	}
}

void UJSCombatComponent::EndHitStun()
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(StunAnimationTimerHandle);
	}
	// Reset default sprite relative location after animation.
	if (const APaperZDCharacter* Owner = Cast<APaperZDCharacter>(GetOwner()))
	{
		if (UPaperFlipbookComponent* MySprite = Owner->GetSprite())
		{
			MySprite->SetRelativeLocation(SpriteDefaultRelativeLocation);			
		}
	}
	
	IsStunned = false;
	OnStunStatusChanged.Broadcast(IsStunned);
}

void UJSCombatComponent::StunAnimation()
{
	// TODO Should this just live on the owner and respond to a broadcast from being hit? Prob.
	if (const APaperZDCharacter* Owner = Cast<APaperZDCharacter>(GetOwner()))
	{
		if (UPaperFlipbookComponent* MySprite = Owner->GetSprite())
		{
			StunAnimation_flipflop = !StunAnimation_flipflop;
			const float x_offset = StunAnimation_flipflop ? StunAnimationTranslation : StunAnimationTranslation * -1.0; 
			const FVector NewRelLocation = SpriteDefaultRelativeLocation + FVector(x_offset, 0, 0);
			MySprite->SetRelativeLocation(NewRelLocation);			
		}
	}
}

void UJSCombatComponent::PlayHitStunSound() const
{
	if (HitStunSoundCue)
	{
		if (const UWorld* World = GetWorld())
		{
			UGameplayStatics::PlaySoundAtLocation(
				World,
				HitStunSoundCue,
				GetOwner()->GetActorLocation(),
				GetOwner()->GetActorRotation()
			);	
		}
	}
}


void UJSCombatComponent::BeginHitPause()
{
	if (AActor* Owner = GetOwner())
	{
		Owner->CustomTimeDilation = 0.0;
	}
	
	if (const UWorld* World = GetWorld())
	{
		FTimerHandle HitPauseTimerHandle; // TODO Maybe keep this ref in .h?
		World->GetTimerManager().SetTimer(
			HitPauseTimerHandle,
			this,
			&UJSCombatComponent::EndHitPause,
			HitPauseDuration,
			false);
	}
	
	IsHitPaused = true;
}

void UJSCombatComponent::EndHitPause()
{
	if (AActor* Owner = GetOwner())
	{
		Owner->CustomTimeDilation = 1.0;
	}
	IsHitPaused = false;
}
