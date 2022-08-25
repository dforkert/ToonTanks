// Copyright Dominik Forkert. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BasePawn.generated.h"

class UCapsuleComponent;
class USoundBase;
class AProjectile;

UCLASS()
class TOONTANKS_API ABasePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABasePawn();

	virtual void HandleDestruction();

protected:
	void RotateTurret(FVector LookAtTarget, const float DeltaTime) const;
	void Fire();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pawn Components", meta=(AllowPrivateAccess="true"))
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pawn Components", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pawn Components", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* TurretMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pawn Components", meta=(AllowPrivateAccess="true"))
	USceneComponent* ProjectileSpawnPoint;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category="Combat")
	UParticleSystem* DeathParticles;

	UPROPERTY(EditAnywhere, Category="Sound")
	USoundBase* DeathSound;

	UPROPERTY(EditAnywhere, Category="Combat")
	TSubclassOf<UCameraShakeBase> DeathCameraShakeClass;
};
