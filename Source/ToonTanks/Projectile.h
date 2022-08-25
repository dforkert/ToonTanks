// Copyright Dominik Forkert. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UProjectileMovementComponent;

UCLASS()
class TOONTANKS_API AProjectile final : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Projectile Components")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, Category="Projectile Components")
	UProjectileMovementComponent* ProjectileMovement;
	
	UPROPERTY(VisibleAnywhere, Category="Projectile Components")
	UParticleSystemComponent* ProjectileSmokeTrail;

	UPROPERTY(EditAnywhere, Category="Combat")
	UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, Category="Sound")
	USoundBase* ProjectileLaunchSound;

	UPROPERTY(EditAnywhere, Category="Sound")
	USoundBase* ProjectileHitSound;

	UPROPERTY(EditAnywhere, Category="Combat")
	TSubclassOf<UCameraShakeBase> HitCameraShakeClass;
	
	UPROPERTY(EditAnywhere, Category="Combat")
	float DamageAmount{50.f};
	
	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
		);

	

	
};
