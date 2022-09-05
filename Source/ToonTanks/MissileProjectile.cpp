// Copyright Dominik Forkert. All Rights Reserved.


#include "MissileProjectile.h"

#include "EigenExternal.h"
#include "Tank.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectileClasses/ProjectileHomingBase.h"

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


	FString Prediction = FEigenExternalModule::GetLMPrediction();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Prediction: ") + Prediction);

	// TODO: Get ActorOfClass is slow!!!!
	const AActor* Target = UGameplayStatics::GetActorOfClass(this, ATank::StaticClass());

	// TODO: refactor this into functions
	if (ProjectileHomingClass)
	{
		ProjectileHoming = NewObject<UProjectileHomingBase>(this, ProjectileHomingClass, TEXT("Projectile Homing Scene Component"));
		ProjectileHoming->RegisterComponent();
		
		ProjectileMovement->HomingTargetComponent = ProjectileHoming;
	}
	else
	{
		ProjectileMovement->HomingTargetComponent = Target->GetRootComponent();
	}
	
}

void AMissileProjectile::DestroyProjectile()
{
	if (ProjectileHoming)
	{
		// TODO: remove?
	}
	Super::DestroyProjectile();
}

// Called every frame
void AMissileProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ProjectileHoming)
	{
		// TODO: Get ActorOfClass is slow!!!!
		const AActor* Target = UGameplayStatics::GetActorOfClass(this, ATank::StaticClass());
		const FVector TargetLocation{Target->GetRootComponent()->GetComponentLocation()};
		
		ProjectileHoming->UpdateProjectileHomingLocation(TargetLocation, ProjectileMovement, DeltaTime);
	}

}
