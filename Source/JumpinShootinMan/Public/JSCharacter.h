#pragma once

#include "CoreMinimal.h"

#include "PaperZDCharacter.h"
#include "Combat/JSCombatInterface.h"
#include "JSCharacter.generated.h"

//TODO Figure out log categories?? For next project maybe?

// If our game was to have NPCs, I'd put the IAPCombatInterface on a APCombatCharacter class maybe?
UCLASS()
class  AJSCharacter : public APaperZDCharacter, public IJSCombatInterface
{
	GENERATED_BODY()

public:
	AJSCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void BeginPlay() override;

	UFUNCTION()
	virtual UJSCombatComponent* GetCombatComponent() const override { return CombatComponent; }

	// You need to add UPROPERTY for TObjectPtrs so that the thing is known to unreal GC
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UJSCombatComponent> CombatComponent;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void DamageFlash(); // TODO - I don't like this here? maybe. but sure for now.
	
private:
	UPROPERTY()
	TSoftObjectPtr<UMaterialInstanceDynamic> CharacterMaterialInstancePtr;
	
	FName FlashMult = "FlashMult"; 
	FTimerHandle DamageFlashMultTimerHandle;
	
	UFUNCTION()
	void ResetFlash();
	
};
