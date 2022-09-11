// Copyright Dominik Forkert. All Rights Reserved.


#include "NonLinearProjectileHoming.h"

#include "GameFramework/ProjectileMovementComponent.h"



void UNonLinearProjectileHoming::InitializeSampleBuffer()
{
	uint32 SampleSize{2};
	FDeltaTimeLocationPair DefaultElement{0.f, FVector::ZeroVector};
	PositionBuffer = MakeUnique<FCircularSampleBuffer>(SampleSize, DefaultElement);
}


void UNonLinearProjectileHoming::BeginPlay()
{
	Super::BeginPlay();

	const TFunctionRef<FVector(FVector InitialVelocity, float InDeltaTime)> ComputeProjectileVelocityFuncRef{
		[this](FVector InitialVelocity, float InDeltaTime)-> FVector
		{
			return ProjectileMovement->ComputeVelocity(InitialVelocity, InDeltaTime);
		}
	};
	LMTargetPredictor.InitializeLMTargetPredictor(ComputeProjectileVelocityFuncRef);
}

// TODO: add warning if DeltaTime < MIN_TICK_TIME (= 1e-6f; protected member in ProjectileMovement Component) or use SMALL_NUMBER macro
void UNonLinearProjectileHoming::UpdateProjectileHomingLocation(
	const float DeltaTime)
 {
	if (ProjectileMovement && PositionBuffer)
	{
		const FVector TargetLocation{TargetActor->GetActorLocation()};

		const float OldDeltaTime =  PositionBuffer->GetElement().Key;
		if (OldDeltaTime > 0)	// Check if there is there is already a position sample in the buffer
			{
			const FVector OldTargetLocation = PositionBuffer->GetElement().Value;
			const FVector ProjectileLocation = ProjectileMovement->GetOwner()->GetActorLocation();
			const FVector ProjectileVelocity = ProjectileMovement->Velocity;

			DrawDebugDirectionalArrow(GetWorld(), ProjectileLocation, ProjectileLocation + ProjectileVelocity, 100.f, FColor::Emerald);

			const FVector PredictedTargetLocation{
				PredictSimpleLinearTargetLocation(
					ProjectileVelocity,
					ProjectileLocation,
					OldTargetLocation,
					TargetLocation,
					DeltaTime)
			};

			////////////// TODO:
			// TFunctionRef<FVector(FVector InitialVelocity, float InDeltaTime)> ComputeProjectileVelocity{
			// 	ProjectileMovement->ComputeVelocity
			// };
			
			///////////////////////
			FVector LMPredictedTargetLocation = LMTargetPredictor.LMPredictTargetLocation(
				ProjectileLocation,
				ProjectileVelocity,
				TargetLocation,
				(TargetLocation - OldTargetLocation)/DeltaTime,
				PredictedTargetLocation
			);
			
			//FString Prediction = FEigenExternalModule::LMPredictTargetLocation(,,,,);
			// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Prediction: ") + Prediction);
			
				
			SetWorldLocation(PredictedTargetLocation);
			
			DrawDebugDirectionalArrow(GetWorld(), TargetLocation, PredictedTargetLocation, 100.f, FColor::Red);
			DrawDebugCrosshairs(GetWorld(), OldTargetLocation, FRotator::ZeroRotator, 300.f, FColor::Green);
			DrawDebugCrosshairs(GetWorld(), PredictedTargetLocation, FRotator::ZeroRotator, 300.f, FColor::Red);
			DrawDebugCrosshairs(GetWorld(), LMPredictedTargetLocation, FRotator::ZeroRotator, 600.f, FColor::Blue);
			
			}
		
		PositionBuffer->PushNewElement(FDeltaTimeLocationPair{DeltaTime, TargetLocation});
	}
 }