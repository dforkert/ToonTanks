// Copyright Dominik Forkert. All Rights Reserved.


#include "MissileProjectile.h"

#include "Tank.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectileHoming//ProjectileHomingBase.h"

// Sets default values
AMissileProjectile::AMissileProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMovement->bIsHomingProjectile = true;
	ProjectileMovement->HomingAccelerationMagnitude = 1000.f;
	ProjectileMovement->InitialSpeed = 200.f;
	
}

// Called when the game starts or when spawned
void AMissileProjectile::BeginPlay()
{
	Super::BeginPlay();


	// TODO: Get ActorOfClass is slow!!!!
	const AActor* Target = UGameplayStatics::GetActorOfClass(this, ATank::StaticClass());

	// TODO: refactor this into functions
	if (ProjectileHomingClass)
	{
		ProjectileHoming = UProjectileHomingBase::SpawnProjectileHomingComponent(this, ProjectileHomingClass, ProjectileMovement, Target);
	}
	else
	{
		ProjectileMovement->HomingTargetComponent = Target->GetRootComponent();
	}
	
}

// Called every frame
void AMissileProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
