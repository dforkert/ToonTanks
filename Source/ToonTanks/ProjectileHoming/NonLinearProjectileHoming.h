// Copyright Dominik Forkert. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EigenExternal.h"
#include "SimpleLinearProjectileHoming.h"
#include "NonLinearProjectileHoming.generated.h"


/**
 * 
 */
UCLASS()
class TOONTANKS_API UNonLinearProjectileHoming : public USimpleLinearProjectileHoming
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	FVector PredictTargetLocation(float DeltaTime, FVector ProjectileLocation, FVector TargetLocation);

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
