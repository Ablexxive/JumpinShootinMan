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

	UFUNCTION()
	virtual UJSCombatComponent* GetCombatComponent() const override { return CombatComponent; }

	// You need to add UPROPERTY for TObjectPtrs so that the thing is known to unreal GC
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UJSCombatComponent> CombatComponent;
};
