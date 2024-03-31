#pragma once

#include "CoreMinimal.h"

#include "JSProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UPaperFlipbook;
class UPaperFlipbookComponent;

UCLASS()
class AJSProjectile : public AActor
{
	GENERATED_BODY()

public:
	AJSProjectile(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USphereComponent> SphereCollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbookComponent> FlipbookComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperFlipbook* VFXFlipbook;

	UFUNCTION(BlueprintCallable)
	void PlayVFXFlipbook();

	UFUNCTION()
	void EndProjectile();
};