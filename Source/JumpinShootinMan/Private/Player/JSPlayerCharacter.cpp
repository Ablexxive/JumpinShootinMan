#include "Player/JSPlayerCharacter.h"

#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PawnMovementComponent.h"

AJSPlayerCharacter::AJSPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bDoCollisionTest = false;
	// Sets absolute rotation to TRUE so that the player controller can rotate but the camera stays facing forward.
	SpringArm->SetAbsolute(false, true, false);

	// Eyeball defaults
	const FRotator SpringArmRotation = FRotator(0.0, -90.0, 0.0);
	SpringArm->SetRelativeRotation(SpringArmRotation);
	SpringArm->TargetArmLength = 750.0;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void AJSPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	MyPlayerControllerPtr  = Cast<APlayerController>(GetController());

	if (!IsValid(MyPlayerControllerPtr.Get()))
	{
		// TODO: Add Logging
		return;
	}
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(MyPlayerControllerPtr->GetLocalPlayer());

	InputSubsystem->ClearAllMappings();
	if (InputMapping == nullptr)
	{
		// TODO: Add Logging
		return;
	}
	InputSubsystem->AddMappingContext(InputMapping, 0);

	// TODO: Is this okay here or in BeginPlay?
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent == nullptr)
	{
		// TODO: Add Logging
		return;
	}

	// TODO - Should I be keeping references to these bindings and then explicitly unbind them?
	if (InputConfig == nullptr)
	{
		// TODO: Add Logging
		return;	
	}
	
	if (InputConfig->InputMove != nullptr)
	{
		EnhancedInputComponent->BindAction(InputConfig->InputMove, ETriggerEvent::Triggered, this, &AJSPlayerCharacter::IC_Move_Triggered);
	}
	
	if (InputConfig->InputJump != nullptr)
	{
		EnhancedInputComponent->BindAction(InputConfig->InputJump, ETriggerEvent::Started, this, &AJSPlayerCharacter::IC_Jump_Started);
		EnhancedInputComponent->BindAction(InputConfig->InputJump, ETriggerEvent::Completed, this, &AJSPlayerCharacter::IC_Jump_Completed);
		EnhancedInputComponent->BindAction(InputConfig->InputJump, ETriggerEvent::Canceled, this, &AJSPlayerCharacter::IC_Jump_Canceled);
	}
	
	if (InputConfig->InputShoot != nullptr)
	{
		EnhancedInputComponent->BindAction(InputConfig->InputShoot, ETriggerEvent::Triggered, this, &AJSPlayerCharacter::IC_Shoot_Triggered);
	}
	
	if (DebugInputConfig == nullptr)
	{
		// TODO: Add Logging
		return;	
	}

	if (DebugInputConfig->DebugInputRestart != nullptr)
	{
		EnhancedInputComponent->BindAction(DebugInputConfig->DebugInputRestart, ETriggerEvent::Triggered, this, &AJSPlayerCharacter::IC_DebugRestart_Triggered);
	}
	
	if (DebugInputConfig->DebugInputToggleImmortal != nullptr)
	{
		EnhancedInputComponent->BindAction(DebugInputConfig->DebugInputToggleImmortal, ETriggerEvent::Triggered, this, &AJSPlayerCharacter::IC_DebugToggleImmortal_Triggered);
	}
}

void AJSPlayerCharacter::IC_Move_Triggered(const FInputActionValue& Value)
{
	if (!CombatComponent->CanAct())
	{
		return;
	}
	if (!IsValid(MyPlayerControllerPtr.Get()))
	{
		// TODO: Logging
		return;
	}
	const FVector2D MoveValue = Value.Get<FVector2D>();

	const FRotator MovementRotation(0, MyPlayerControllerPtr->GetControlRotation().Yaw, 0);

	// /Up/Down direction
	if (MoveValue.Y != 0.0f)
	{
		const	FVector Direction = MovementRotation.RotateVector(GetActorRightVector() * -1.0);
		AddMovementInput(Direction, MoveValue.Y);
	}

	// Left/Right direction
	if (MoveValue.X != 0.0f)
	{
		const	FVector Direction = MovementRotation.RotateVector(GetActorForwardVector());
		AddMovementInput(Direction, MoveValue.X);

		// Update control rotation so that sprite properly faces movement direction.
		const UPawnMovementComponent* MyMovementComponent = GetMovementComponent();
		if (IsValid(MyMovementComponent))
		{
			if (MoveValue.X > 0.0f)
			{
				const FRotator NewRotation(0.0, 0.0, 0.0);
				MyPlayerControllerPtr->SetControlRotation(NewRotation);
			}
			else if (MoveValue.X < 0.0f)
			{
				const FRotator NewRotation(0.0, 180.0, 0.0);
				MyPlayerControllerPtr->SetControlRotation(NewRotation);
			}
		}
	}
}

void AJSPlayerCharacter::IC_Jump_Started(const FInputActionValue& Value)
{
	if (CombatComponent->CanAct())
	{
		Jump();
	}
};

void AJSPlayerCharacter::IC_Jump_Completed(const FInputActionValue& Value)
{
	StopJumping();
};

void AJSPlayerCharacter::IC_Jump_Canceled(const FInputActionValue& Value)
{
	StopJumping();
};

void AJSPlayerCharacter::IC_Shoot_Triggered(const FInputActionValue& Value)
{
	UJSCombatComponent* MyCombatComponent = CombatComponent.Get();
	if (MyCombatComponent == nullptr)
	{
		// TODO Log error
		return;
	}
	
	if (MyCombatComponent->IsAttacking || !MyCombatComponent->CanAct())
	{
		return;
	}

	// We are using Shoot_Started in the player BP to do the attack, don't need to call here.
	// Probably can just delete this function? But will revisit when adding a charge shot and maybe
	// a melee shot.
	//MyCombatComponent->DoAttack();
}

void AJSPlayerCharacter::IC_DebugRestart_Triggered(const FInputActionValue& Value)
{
	if (MyPlayerControllerPtr.Get())
	{
		MyPlayerControllerPtr->RestartLevel();
	}	
}

void AJSPlayerCharacter::IC_DebugToggleImmortal_Triggered(const FInputActionValue& Value)
{
	//if (UJSCombatComponent* MyCombatComponent = CombatComponent.Get())
	if (CombatComponent.Get())
	{
		CombatComponent->DebugImmortal = !CombatComponent->DebugImmortal;
	}
};
