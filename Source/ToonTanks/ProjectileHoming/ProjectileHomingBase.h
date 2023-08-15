// Copyright Dominik Forkert. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileHomingBase.generated.h"

DECLARE_STATS_GROUP(TEXT("ProjectileHomingBase"), STATGROUP_ProjectileHomingBase, STATCAT_Advanced);



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
	 * @brief Spawns a UProjectileHomingBase component which serves as homing target for a projectile
	 * @param Outer The outer UObject for this component
	 * @param ProjectileHomingClass The ProjectileHomingClass specifies the homing method to be used
	 * @param InProjectileMovement The UProjectileMovementComponent of the projectile actor
	 * @param InTargetActor The target for the homing projectile
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
