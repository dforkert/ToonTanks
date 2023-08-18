// Copyright Dominik Forkert. All Rights Reserved.


#include "BasePawn.h"

#include "Projectile.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABasePawn::ABasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	RootComponent = CapsuleComponent;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(CapsuleComponent);
	
	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh->SetupAttachment(BaseMesh);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(TurretMesh);
}

void ABasePawn::DestroyPawn()
{
	if (DeathParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			this,
			DeathParticles,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	if (DeathCameraShakeClass)
	{
		GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(DeathCameraShakeClass);
	}
}


/**
 * @brief Rotates the Turret in XY-plane to target at the given world coordinates.
 * @param LookAtTarget Target in world coordinates
 * @param DeltaTime Time between consecutive ticks
 */
void ABasePawn::RotateTurret(const FVector& LookAtTarget, const float DeltaTime) const
{
	if (TurretMesh->Mobility == EComponentMobility::Movable)
	{
		const FVector ToTarget{LookAtTarget - TurretMesh->GetComponentLocation()};
		const FRotator LookAtRotation{TurretMesh->GetComponentRotation()};
		const FVector ToTargetLoc = LookAtRotation.UnrotateVector(ToTarget);

		FRotator LookAtRotationLoc = TurretMesh->GetRelativeRotation();
		LookAtRotationLoc.Yaw = ToTargetLoc.Rotation().Yaw;
		
		TurretMesh->SetRelativeRotation(FMath::RInterpTo(
				TurretMesh->GetRelativeRotation(),
				LookAtRotationLoc,
				DeltaTime,
				15.f));
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
/**
 * @brief Spawns a projectile at the ProjectileSpawnPoint of the BasePawn
 */
void ABasePawn::Fire()
{
	const FVector Location = ProjectileSpawnPoint->GetComponentLocation();
	const FRotator Rotation = ProjectileSpawnPoint->GetComponentRotation();
	AProjectile* Projectile{
		GetWorld()->SpawnActor<AProjectile>(
			ProjectileClass,
			Location,
			Rotation
		)
	};
	Projectile->SetOwner(this);
}

