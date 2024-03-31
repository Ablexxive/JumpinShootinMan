#pragma once

#include "Combat/JSProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"

AJSProjectile::AJSProjectile(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SphereCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollisionComponent"));
	SetRootComponent(SphereCollisionComponent);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->ProjectileGravityScale = 0.0;
	ProjectileMovementComponent->InitialSpeed = 1000;
	ProjectileMovementComponent->MaxSpeed = 1000;

	FlipbookComponent = CreateOptionalDefaultSubobject<UPaperFlipbookComponent>(TEXT("FlipbookComponent"));
	FlipbookComponent->SetupAttachment(RootComponent);
	FlipbookComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlipbookComponent->OnFinishedPlaying.AddDynamic(this, &AJSProjectile::EndProjectile);
}

// Switches to VFX Flipbook and turns off looping
void AJSProjectile::PlayVFXFlipbook()
{
	ProjectileMovementComponent->StopMovementImmediately();
	if (IsValid(VFXFlipbook))
	{
		FlipbookComponent->SetFlipbook(VFXFlipbook);
	}
	
	// After the VFX loops, it should trigger "OnFinishedPlaying" and call bound EndProjectile
	FlipbookComponent->SetLooping(false);
};

void AJSProjectile::EndProjectile()
{
	Destroy();	
}

