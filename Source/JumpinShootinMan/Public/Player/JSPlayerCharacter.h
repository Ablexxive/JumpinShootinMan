#pragma once

#include "CoreMinimal.h"

#include "JSCharacter.h"
#include "JSInputConfig.h"
#include "JSPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;

UCLASS()
class AJSPlayerCharacter : public AJSCharacter
{
	GENERATED_BODY()

public:
	AJSPlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UCameraComponent> Camera;

#pragma region // input
private:
	/* Maps key inputs to input actions defined in InputConfig */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* InputMapping;

	/*	My datastruct that defines Move/Jump/Ect. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UJSInputConfig* InputConfig;

	TWeakObjectPtr<APlayerController> MyPlayerControllerPtr; 

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/* Input Config functions. */
	void IC_Move_Triggered(const FInputActionValue& Value);
	void IC_Jump_Started(const FInputActionValue& Value);
	void IC_Jump_Canceled(const FInputActionValue& Value);
	void IC_Shoot_Triggered(const FInputActionValue& Value);
#pragma endregion
};