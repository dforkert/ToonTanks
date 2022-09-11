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
	
	virtual void UpdateProjectileHomingLocation(
		const float DeltaTime
	) override;

private:
	FEigenExternalModule::FLMTargetPredictor LMTargetPredictor;
	
	/**
	 * @brief Initializes SampleBuffer pointing to an FCircularSampleBuffer object of specified size;
	 */
	virtual void InitializeSampleBuffer() override;
	
};
