// Copyright Dominik Forkert. All Rights Reserved.


#include "NonLinearProjectileHoming.h"

#include "GameFramework/ProjectileMovementComponent.h"



void UNonLinearProjectileHoming::InitializeSampleBuffer()
{
	uint32 SampleSize{3};
	FDeltaTimeLocationPair DefaultElement{0.f, FVector::ZeroVector};
	PositionBuffer = MakeUnique<FCircularSampleBuffer>(SampleSize, DefaultElement);
}


void UNonLinearProjectileHoming::BeginPlay()
{
	Super::BeginPlay();

	check(IsValid(ProjectileMovement));
	// TODO: make this a function template to get rid of the FVector <-> VectorXd conversions
	
	FEigenExternalModule::FLMTargetPredictor::FLMResidualFunction LMResidualFunction{
		[this](const FVector UnitDirFromProjectileToTarget, const double Time)-> FVector
		{
			const FVector TargetLocation_Time{SimulateTargetLocation(Time)};
			const FVector ProjectileLocation_Time{SimulateProjectileLocation(UnitDirFromProjectileToTarget, Time)};

			DrawDebugCrosshairs(GetWorld(), TargetLocation_Time, FRotator::ZeroRotator, 500.f, FColor::Yellow);
			DrawDebugCrosshairs(GetWorld(), ProjectileLocation_Time, FRotator::ZeroRotator, 50.f, FColor::Orange, false, 0.5);

			
			return TargetLocation_Time - ProjectileLocation_Time;
		}
	};
	
	// ComputeProjectileVelocityFuncRef will be consumed by InitializeLMTargetPredictor
	LMTargetPredictor.InitializeLMTargetPredictor(LMResidualFunction);

}

FVector UNonLinearProjectileHoming::SimulateTargetLocation(const double Time) const
{
	// TODO: improve structure of PositionBuffer!
	TStaticArray<float, 3> DeltaTime;
	TStaticArray<FVector, 3> TargetLocation;
	for (int32 i = 0; i < 3; i++)
	{
		const TPair<float, FVector> BufferElement {PositionBuffer->GetElement(i - 2)};
		DeltaTime[i] = BufferElement.Key;
		TargetLocation[i] = BufferElement.Value;
	}

	const FVector TargetVelocity_0to1{(TargetLocation[1] - TargetLocation[0]) / DeltaTime[1]};
	const FVector TargetVelocity_1to2{(TargetLocation[2] - TargetLocation[1]) / DeltaTime[2]};

	const FVector TargetAcceleration{(TargetVelocity_1to2 - TargetVelocity_0to1) * 2 / (DeltaTime[1] + DeltaTime[2])};

	const FVector TargetLocation_Time{TargetLocation[2] + TargetVelocity_1to2*Time + TargetAcceleration * Time*Time / 2};

	/*
	const double CurrentDeltaTime{PositionBuffer->GetElement().Key};
	
	const FVector CurrentTargetLocation{PositionBuffer->GetElement().Value};
	const FVector OldTargetLocation{PositionBuffer->GetElement(-1).Value};
	
	const FVector CurrentTargetVelocity{(CurrentTargetLocation - OldTargetLocation) / CurrentDeltaTime};
	
	// x_t = x_0 + v_0 * t
	const FVector TargetLocation_Time{CurrentTargetLocation + CurrentTargetVelocity*Time};
	*/
	
	return TargetLocation_Time;
}

FVector UNonLinearProjectileHoming::SimulateProjectileLocation(const FVector UnitDirFromProjectileToTarget, const double Time) const
{
	const FVector ProjectileLocation_0{ProjectileMovement->GetOwner()->GetActorLocation()};
	const FVector ProjectileVelocity_0{ProjectileMovement->Velocity};
			
	FVector ProjectileAcceleration{UnitDirFromProjectileToTarget * ProjectileMovement->HomingAccelerationMagnitude};
	ProjectileAcceleration.Z += ProjectileMovement->GetGravityZ();

	const FVector ProjectileVelocity_Time{ProjectileMovement->LimitVelocity(ProjectileVelocity_0 + ProjectileAcceleration*Time)};
	// Velocity verlet: x_t = x_0 + (v_0 + v_t) * t/2 
	const FVector ProjectileLocation_Time{ProjectileLocation_0 + (ProjectileVelocity_0 + ProjectileVelocity_Time)*( 0.5 * Time)};
	
	return ProjectileLocation_Time;
}


FVector UNonLinearProjectileHoming::PredictTargetLocation(
	const float DeltaTime,
	const FVector ProjectileLocation,
	const FVector TargetLocation
) const
{
	const float OldDeltaTime = PositionBuffer->GetElement(-2).Key;
	if (OldDeltaTime > 0) // Check if there is there are already two position samples from previous ticks in the buffer
	{
		const FVector OldTargetLocation = PositionBuffer->GetElement(-1).Value;
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

		FVector PredictedUnitDirFromProjectileToTarget;
		double PredictedTime;
		Tie(PredictedUnitDirFromProjectileToTarget, PredictedTime) = LMTargetPredictor.LMPredictTargetLocation(
			ProjectileLocation,
			ProjectileVelocity.Length(),
			PredictedTargetLocationGuess
		);
		const FVector LMPredictedTargetLocation{
			SimulateProjectileLocation(PredictedUnitDirFromProjectileToTarget, PredictedTime)
		};

		#if WITH_EDITOR
			DrawDebugCrosshairs(GetWorld(), TargetLocation, FRotator::ZeroRotator, 200.f, FColor::Green);
			DrawDebugCrosshairs(GetWorld(), PredictedTargetLocationGuess, FRotator::ZeroRotator, 200.f, FColor::Blue);
			DrawDebugCrosshairs(GetWorld(), LMPredictedTargetLocation, FRotator::ZeroRotator, 200.f, FColor::Red);
		#endif

		return LMPredictedTargetLocation;
	}
	else
	{
		return TargetLocation;
	}
}

void UNonLinearProjectileHoming::UpdateProjectileHomingLocation(const float DeltaTime)
{
	ensure(DeltaTime > SMALL_NUMBER);
	ensure(ProjectileMovement && PositionBuffer);

	if (ProjectileMovement && PositionBuffer)
	{
		const FVector ProjectileLocation = ProjectileMovement->GetOwner()->GetActorLocation();
		const FVector TargetLocation{TargetActor->GetActorLocation()};

		PositionBuffer->PushNewElement(FDeltaTimeLocationPair{DeltaTime, TargetLocation});

		if (FVector::Dist(ProjectileLocation, TargetLocation) < MinLMPredictionDistance)
		{
			SetWorldLocation(TargetLocation);
		}
		else
		{
			const FVector PredictedTargetLocation{
				PredictTargetLocation(DeltaTime, ProjectileLocation, TargetLocation)
			};
			SetWorldLocation(PredictedTargetLocation);
		}
	}
}
