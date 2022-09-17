// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once


#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"



struct FLMWrapper;


class EIGENEXTERNAL_API FEigenExternalModule final : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	class EIGENEXTERNAL_API FLMTargetPredictor
	{
	public:
		// Definition of constructor and destructor is deferred until LM is of complete type;
		// required for opaque pointer to LMWrapper
		FLMTargetPredictor();
		~FLMTargetPredictor();
		
		// class contains a TUniquePtr and is therefore not copy constructable/assignable
		FLMTargetPredictor(const FLMTargetPredictor&) = delete;
		FLMTargetPredictor& operator=(const FLMTargetPredictor&) = delete;
		
		// double DeltaTime improves numerical stability of the LM-algorithm
		using FLMResidualFunction = TFunction<FVector(FVector UnitDirFromProjectileToTarget, double Time)>;
		/**
		 * @brief Initializes the member LMWrapper
		 * @param InComputeProjectileVelocityFunc TFunctionRef to the a function which computes the projectile velocity
		 * @param MaxFunctionEvaluations maximum of allowed function evaluations when executing the LM-algorithm
		 * @param ResidualErrorTolerance error tolerance for the residual function 
		 */
		void InitializeLMTargetPredictor(
			FLMResidualFunction& InComputeProjectileVelocityFunc,
			const uint32 MaxFunctionEvaluations = 100u,
			const double ResidualErrorTolerance = 0.1
		);
		
		/**
		 * @brief Predicts a (moving) target's location with an LM-algorithm
		 * @param CurrentProjectileLocation 
		 * @param CurrentProjectileSpeed 
		 * @param PredictedTargetLocationGuess A rough guess for the predicted target location; serves as starting value for the LM-algorithm
		 * @return TTuple consisting of the predicted unit direction vector from projectile to target and time until projectile impact
		 */
		TPair<FVector, double> LMPredictTargetLocation(
			FVector CurrentProjectileLocation,
			double CurrentProjectileSpeed,
			FVector PredictedTargetLocationGuess
		) const;

	private:
		TUniquePtr<FLMWrapper> LMWrapper;
		
	};

	
	
};
