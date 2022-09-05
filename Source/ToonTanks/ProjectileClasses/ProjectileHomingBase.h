// Copyright Dominik Forkert. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileHomingBase.generated.h"

class UProjectileMovementComponent;
/**
 * @brief provides a target for homing AProjectile objects; this base class implements a simple memory-less pursuit model   
 */
UCLASS(ClassGroup=(Custom))
class TOONTANKS_API UProjectileHomingBase : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	UProjectileHomingBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category="Homing")
	USceneComponent* ProjectileHomingSceneComponent;


public:
	/**
	 * @brief Updates the location of the ProjectileHoming Actor
	 * @param TargetLocation location where the ProjectileHoming Actor will be moved to
	 * @param ProjectileMovement Pointer to the owning projectile
	 * @param DeltaTime
	 */
	virtual void UpdateProjectileHomingLocation(const FVector TargetLocation, UProjectileMovementComponent const* ProjectileMovement, const float DeltaTime);
};
