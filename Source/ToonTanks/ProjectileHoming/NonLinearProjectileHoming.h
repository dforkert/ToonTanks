// Copyright Dominik Forkert. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EigenExternal.h"
#include "SimpleLinearProjectileHoming.h"
#include "NonLinearProjectileHoming.generated.h"

/**
 * @brief provides a target for homing AProjectile objects; this class implements a non-linear prediction model   
 * based on a non-linear prediction of the projectile's and target's future trajectories, which are solved by an LM-algorithm
 */
UCLASS()
class TOONTANKS_API UNonLinearProjectileHoming : public USimpleLinearProjectileHoming
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	/**
	 * @brief Simulates the target location at given Time based on a 2nd-order taylor approximation of the target's past trajectory
	 * @param Time 
	 * @return Simulated target location at Time
	 */
	FVector SimulateTargetLocation(double Time) const;
	
	/**
	 * @brief Simulates the projectile location at given UnitDirFromProjectileToTarget and Time based on a velocity verlet approximation
	 * @param UnitDirFromProjectileToTarget Unit vector representing the direction from projectile to target
	 * @param Time 
	 * @return Simulated projectile location at Time
	 */
	FVector SimulateProjectileLocation(const FVector& UnitDirFromProjectileToTarget, double Time) const;

	/**
	 * @brief Predicts a future target location at time of projectile impact using an LM-algorithm
	 * @param DeltaTime Current DeltaTime
	 * @param ProjectileLocation Current projectile location
	 * @param TargetLocation Current target location
	 * @return predicted future target location at time of projectile impact 
	 */
	FVector PredictTargetLocation(float DeltaTime, const FVector& ProjectileLocation, const FVector& TargetLocation) const;

	/**
	 * @brief Updates the homing target of the projectile using an LM-algorithm
	 * @param DeltaTime Current DeltaTime
	 */
	virtual void UpdateProjectileHomingLocation(
		const float DeltaTime
	) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Homing",DisplayName="Minimum Distance to target for LM-Predictor to be used")
	float MinLMPredictionDistance{200.f};
	
	FEigenExternalModule::FLMTargetPredictor LMTargetPredictor;
	
	/**
	 * @brief Initializes SampleBuffer pointing to an FCircularSampleBuffer object of specified size;
	 */
	virtual void InitializeSampleBuffer() override;
	
};
