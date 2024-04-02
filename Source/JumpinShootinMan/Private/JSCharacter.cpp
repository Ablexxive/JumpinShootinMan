#include "JSCharacter.h"

#include "PaperFlipbookComponent.h"

AJSCharacter::AJSCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	CombatComponent = CreateDefaultSubobject<UJSCombatComponent>(TEXT("Combat Component"));
}

void AJSCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UPaperFlipbookComponent* MySprite = GetSprite())
	{
		CharacterMaterialInstancePtr = MySprite->CreateDynamicMaterialInstance(0);
	}
}

void AJSCharacter::DamageFlash()
{
	if (UMaterialInstanceDynamic* MyMaterial = CharacterMaterialInstancePtr.Get())
	{
		MyMaterial->SetScalarParameterValue(FlashMult, 10);
		
		if (const UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				DamageFlashMultTimerHandle,
				this,
				&AJSCharacter::ResetFlash,
				0.2,
				false);
		}
	}
}

void AJSCharacter::ResetFlash()
{
	if (UMaterialInstanceDynamic* MyMaterial = CharacterMaterialInstancePtr.Get())
	{
		MyMaterial->SetScalarParameterValue(FlashMult, 0);
	}
}