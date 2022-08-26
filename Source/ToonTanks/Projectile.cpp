// Copyright Dominik Forkert. All Rights Reserved.


#include "Projectile.h"
#include "Projectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	RootComponent = ProjectileMesh;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->MaxSpeed = 1300.f;
	ProjectileMovement->InitialSpeed = 1300.f;

	ProjectileSmokeTrail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Projectile Smoke Trail"));
	ProjectileSmokeTrail->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Bind functions to Component Hit Event
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

	if (ProjectileLaunchSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ProjectileLaunchSound, GetActorLocation());
	}
}

void AProjectile::OnHit(
	UPrimitiveComponent* /*HitComp*/,
	AActor* OtherActor,
	UPrimitiveComponent* /*OtherComp*/,
	FVector /*NormalImpulse*/,
	const FHitResult& /*Hit*/
	)
{
	const AActor* ProjectileOwner{GetOwner()};
	// If there is no ProjectileOwner (e.g. the Owner was destroyed in the mean time),
	// destroy the particle as well 
	if (!ProjectileOwner)
	{
		Destroy();
		return;
	}
	
	AController* ProjectileOwnerInstigator{ProjectileOwner->GetInstigatorController()};
	UClass* DamageTypeClass{UDamageType::StaticClass()};

	if (OtherActor && OtherActor != this && OtherActor != ProjectileOwner)
	{
		UGameplayStatics::ApplyDamage(
			OtherActor,
			DamageAmount,
			ProjectileOwnerInstigator,
			this,
			DamageTypeClass
		);

		if (HitParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				this,
				HitParticles,
				GetActorLocation(),
				GetActorRotation()
			);
		}

		if (ProjectileHitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ProjectileHitSound, GetActorLocation());
		}

		if(HitCameraShakeClass)
		{
			GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(HitCameraShakeClass);
		}
	}
	
	Destroy();
}


