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

	/**
	 * @brief 
	 * @param Outer 
	 * @param ProjectileHomingClass 
	 * @param InProjectileMovement 
	 * @param InTargetActor 
	 * @return 
	 */
	static UProjectileHomingBase* SpawnProjectileHomingComponent(
		UObject* Outer,
		const UClass* ProjectileHomingClass,
		UProjectileMovementComponent* InProjectileMovement,
		const AActor* InTargetActor
	);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	UProjectileMovementComponent* ProjectileMovement;
	
	TWeakObjectPtr<const AActor> TargetActor;


private:
	UPROPERTY(VisibleAnywhere, Category="Homing")
	USceneComponent* ProjectileHomingSceneComponent;




	
	/**
	 * @brief Updates the location of the ProjectileHoming Actor
	 * @param TargetLocation location where the ProjectileHoming Actor will be moved to
	 * @param ProjectileMovement Pointer to the owning projectile
	 * @param DeltaTime
	 */
	virtual void UpdateProjectileHomingLocation(const float DeltaTime);

public:
	// Called every frame
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
		) override;

};
