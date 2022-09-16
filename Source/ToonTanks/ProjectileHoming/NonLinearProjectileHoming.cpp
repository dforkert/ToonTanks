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

	check(IsValid(ProjectileMovement));
	// TODO: make this a function template to get rid of the FVector <-> VectorXd conversions
	// TODO: include the complete fvec definition in this function
	FEigenExternalModule::FLMTargetPredictor::FSimulateProjectileLocationFunc SimulateProjectileLocationFunc{
		[this](FVector UnitDirFromProjectileToTarget, double Time)-> FVector
		{
			const FVector ProjectileLocation_0 = ProjectileMovement->GetOwner()->GetActorLocation();
			const FVector ProjectileVelocity_0 = ProjectileMovement->Velocity;
			
			FVector ProjectileAcceleration = UnitDirFromProjectileToTarget * ProjectileMovement->HomingAccelerationMagnitude;
			ProjectileAcceleration.Z += ProjectileMovement->GetGravityZ();

			const FVector ProjectileVelocity_Time = ProjectileMovement->LimitVelocity(ProjectileVelocity_0 + ProjectileAcceleration*Time);
			// Velocity verlet: x_t = x_0 + (v_0 + v_t) * t/2 
			return ProjectileLocation_0 + (ProjectileVelocity_0 + ProjectileVelocity_Time)*( 0.5 * Time);
		}
	};
	// TODO: convert InitializeLMTargetPredictor into a template with typenames for FVector and VectorXd?
	// ComputeProjectileVelocityFuncRef will be consumed by InitializeLMTargetPredictor
	LMTargetPredictor.InitializeLMTargetPredictor(SimulateProjectileLocationFunc);

}

// TODO: add warning if DeltaTime < MIN_TICK_TIME (= 1e-6f; protected member in ProjectileMovement Component) or use SMALL_NUMBER macro
void UNonLinearProjectileHoming::UpdateProjectileHomingLocation(const float DeltaTime)
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
			const FVector PredictedTargetLocationGuess{
				PredictSimpleLinearTargetLocation(
					ProjectileVelocity,
					ProjectileLocation,
					OldTargetLocation,
					TargetLocation,
					DeltaTime
					)
			};
			FVector LMPredictedTargetLocation = LMTargetPredictor.LMPredictTargetLocation(
				ProjectileLocation,
				ProjectileVelocity,
				TargetLocation,
				(TargetLocation - OldTargetLocation)/DeltaTime,
				PredictedTargetLocationGuess
			); 
			SetWorldLocation(LMPredictedTargetLocation);

			#if WITH_EDITOR
				DrawDebugCrosshairs(GetWorld(), TargetLocation, FRotator::ZeroRotator, 200.f, FColor::Green);
				DrawDebugCrosshairs(GetWorld(), PredictedTargetLocationGuess, FRotator::ZeroRotator, 200.f, FColor::Blue);
				DrawDebugCrosshairs(GetWorld(), LMPredictedTargetLocation, FRotator::ZeroRotator, 200.f, FColor::Red);
			#endif
			}
		
		PositionBuffer->PushNewElement(FDeltaTimeLocationPair{DeltaTime, TargetLocation});
	}
 }