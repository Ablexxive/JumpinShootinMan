#pragma once

#include "CoreMinimal.h"
#include "JSCombatComponent.h"
#include "UObject/Interface.h"

#include "JSCombatInterface.generated.h"

UINTERFACE(BlueprintType, MinimalAPI, Category = "Combat", meta = (CannotImplementInterfaceInBlueprint))
class UJSCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class IJSCombatInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual UJSCombatComponent* GetCombatComponent() const = 0;
	
	/*
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Combat")
	void ToggleAttackHitbox(bool enabled);
	*/

};
