// Copyright Dominik Forkert. All Rights Reserved.


#include "ProjectileHomingBase.h"


// Sets default values
UProjectileHomingBase::UProjectileHomingBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void UProjectileHomingBase::BeginPlay()
{
	Super::BeginPlay();
	
}


void UProjectileHomingBase::UpdateProjectileHomingLocation(const FVector TargetLocation, UProjectileMovementComponent const* ProjectileMovement, const float DeltaTime)
{
	SetWorldLocation(TargetLocation);
	DrawDebugCrosshairs(GetWorld(), GetComponentLocation(), FRotator::ZeroRotator, 300.f, FColor::Red);
}
