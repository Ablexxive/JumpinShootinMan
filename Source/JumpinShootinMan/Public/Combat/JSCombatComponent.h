#pragma once

#include "CoreMinimal.h"
#include "PaperZDAnimInstance.h"
#include "Components/ActorComponent.h"

#include "JSCombatComponent.generated.h"


UENUM(BlueprintType)
enum class EFaction : uint8
{
	Player = 0	UMETA(DisplayName = "Player"),
	Enemy = 1	UMETA(DisplayName = "Enemy")
};

class UBoxComponent;
class USoundCue;

DECLARE_DELEGATE_OneParam(FJSOnAttackEndSignature, bool /* bCompleted */);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class  UJSCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UJSCombatComponent();
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCombatActorDefeated);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStunStatusChanged, bool, IsStunned);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	UPROPERTY(BlueprintAssignable)
	FOnCombatActorDefeated OnCombatActorDefeated;
	
	UPROPERTY(BlueprintAssignable)
	FOnStunStatusChanged OnStunStatusChanged;

	//UPROPERTY(BlueprintAssignable)
	FJSOnAttackEndSignature OnAttackEndSignature;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat|Attack")
	uint8 AttackDamage = 30;
	
	UPROPERTY(EditAnywhere, Category = "Combat|Animation")
	FName AttackHitboxName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animation")
	UPaperZDAnimSequence* AttackAnimSequence;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animation")
	FName AttackAnimSequenceSlot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat|Character|Runtime")
	bool IsAttacking = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat|Character|Runtime")
	bool IsStunned = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat|Character|Runtime")
	bool IsHitPaused = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat|Character|Runtime")
	bool IsDead = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animation")
	FName ABPJumpName_HitStun;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animation")
	FName ABPJumpName_Dead;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Character")
	float ShootAnimationDuration = 0.4;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Character")
	float HitPauseDuration = 0.2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Character")
	float StunDuration = 0.5;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Character")
	float StunAnimationTranslation = 5.0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Character")
	float StunAnimationLoop = 0.05;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat|Attack")
	uint8 FoundAttackHitBox = 0;

	UFUNCTION(BlueprintCallable, Category="Combat|Attack")
	void DoAttack();
	
	UFUNCTION(BlueprintCallable, Category="Combat|Attack")
	void AttackAnimationComplete();
	//void AttackAnimationComplete(bool Success); // Pair together for ANS

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Projectile")
	float MaxShotEnergy = 3.00;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Projectile")
	float ShotEnergy = 3.00;

	UFUNCTION(BlueprintCallable, Category="Combat|Projectile")
	void RefundShotEnergy(float ShotCost);

	// Returns False if not enough energy to spend.
	UFUNCTION(BlueprintCallable, Category="Combat|Projectile")
	bool SpendShotEnergy(float ShotCost);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat|Character")
	int32 MaxHealth = 100;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Combat|Character|Runtime")
	int32 CurrentHealth = 100;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat|Character")
	EFaction MyFaction = EFaction::Player;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat|Character")
	USoundCue* HitStunSoundCue;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ToggleAttackHitbox(bool Enabled);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void TakeDamage(uint8 InDamage);

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool CanAct() const;
	
private:
	TWeakObjectPtr<UBoxComponent> AttackHitBoxPtr;
	
	TWeakObjectPtr<UPaperZDAnimInstance> AnimInstancePtr;
	
	FZDOnAnimationOverrideEndSignature AttackAnimationOverideDelegate;
//	void AttackAnimationComplete(bool Success); - Making public for use in ANS

	FTimerHandle StunAnimationTimerHandle;
	bool StunAnimation_flipflop = false;
	FVector SpriteDefaultRelativeLocation;

	FTimerHandle ShootAnimationTimerHandle;

	UFUNCTION()
	void BeginAttackHitboxOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* Other,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	void BeginHitStun();
	
	void EndHitStun();

	void StunAnimation();
	
	void PlayHitStunSound() const;
	
	void BeginHitPause();
	
	void EndHitPause();

};
