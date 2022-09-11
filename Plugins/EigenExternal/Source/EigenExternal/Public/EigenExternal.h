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
		
		
		using FComputeProjectileVelocityFuncRef = TFunctionRef<FVector(FVector InitialVelocity, float InDeltaTime)>;
		/**
		 * @brief Initializes the member LMWrapper
		 * @param InComputeProjectileVelocityFuncRef TFunctionRef to the a function which computes the projectile velocity
		 * @param MaxFunctionEvaluations maximum of allowed function evaluations when executing the LM-algorithm
		 */
		void InitializeLMTargetPredictor(
			const FComputeProjectileVelocityFuncRef& InComputeProjectileVelocityFuncRef,
			const uint32 MaxFunctionEvaluations = 2000u
		);
		
		FVector LMPredictTargetLocation(
			FVector InCurrentProjectileLocation,
			FVector InCurrentProjectileVelocity,
			FVector InCurrentTargetLocation,
			FVector InCurrentTargetVelocity,
			FVector InPredictedTargetLocationGuess
		);

	private:
		TUniquePtr<FLMWrapper> LMWrapper;
		
	};

	
	
};
