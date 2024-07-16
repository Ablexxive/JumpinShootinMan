#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputAction.h"

#include "JSInputConfig.generated.h"

UCLASS()
class UJSInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputMove;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputJump;
	
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	//UInputAction* InputAttack;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputShoot;
};

UCLASS()
class UJSDebugInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* DebugInputRestart;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* DebugInputToggleImmortal;
};
