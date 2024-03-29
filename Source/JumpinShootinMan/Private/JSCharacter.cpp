#include "JSCharacter.h"

AJSCharacter::AJSCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	CombatComponent = CreateDefaultSubobject<UJSCombatComponent>(TEXT("Combat Component"));
}
