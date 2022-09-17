// Copyright Dominik Forkert. All Rights Reserved.


#include "NonLinearProjectileHoming.h"

#include "GameFramework/ProjectileMovementComponent.h"



void UNonLinearProjectileHoming::InitializeSampleBuffer()
{
	uint32 SampleSize{3};
	FDeltaTimeLocationPair DefaultElement{0.f, FVector::ZeroVector};
	DeltaTimeLocationBuffer = MakeUnique<FCircularDeltaTimeLocationBuffer>(SampleSize, DefaultElement);
}


void UNonLinearProjectileHoming::BeginPlay()
{
	Super::BeginPlay();

	FEigenExternalModule::FLMTargetPredictor::FLMResidualFunction LMResidualFunction{
		[this](const FVector UnitDirFromProjectileToTarget, const double Time)-> FVector
		{
			const FVector TargetLocation_Time{SimulateTargetLocation(Time)};
			const FVector ProjectileLocation_Time{SimulateProjectileLocation(UnitDirFromProjectileToTarget, Time)};
			
			return TargetLocation_Time - ProjectileLocation_Time;
		}
	};
	
	// ComputeProjectileVelocityFuncRef will be consumed by InitializeLMTargetPredictor
	LMTargetPredictor.InitializeLMTargetPredictor(LMResidualFunction);

}

FVector UNonLinearProjectileHoming::SimulateTargetLocation(const double Time) const
{
	check(DeltaTimeLocationBuffer.IsValid());
	ensure(DeltaTimeLocationBuffer->GetCapacity() >= 3);

	[[maybe_unused]] double UnusedDeltaTime;
	double DeltaTime_0To1, DeltaTime_1To2;	// DeltaTime_[s]To[t]: DeltaTime between frames [s] and [t]
	FVector TargetLocation_0, TargetLocation_1, TargetLocation_2;	// TargetLocation_[t]: TargetLocation at frame [t] 

	Tie(UnusedDeltaTime, TargetLocation_0) = DeltaTimeLocationBuffer->GetElement(-2);
	Tie(DeltaTime_0To1, TargetLocation_1) = DeltaTimeLocationBuffer->GetElement(-1);
	Tie(DeltaTime_1To2, TargetLocation_2) = DeltaTimeLocationBuffer->GetElement();

	// forward difference quotients for the velocity v_0
	const FVector TargetVelocity_0To1{(TargetLocation_1 - TargetLocation_0) / DeltaTime_0To1};
	// forward difference quotients for the velocity v_1
	const FVector TargetVelocity_1To2{(TargetLocation_2 - TargetLocation_1) / DeltaTime_1To2};
	// central difference quotients for the velocity v_1
	const FVector TargetVelocity_0To2{(TargetLocation_2 - TargetLocation_0) / (DeltaTime_0To1 + DeltaTime_1To2)};

	// central difference quotient for the acceleration a_1
	const FVector TargetAcceleration_0To2{(TargetVelocity_1To2 - TargetVelocity_0To1) * 2 / (DeltaTime_0To1 + DeltaTime_1To2)};

	// 2nd order taylor approximation based on central difference quotients:
	// x_t = x_1 + v_1 * t + a_1 * t^2 / 2
	const FVector TargetLocation_Time{TargetLocation_1 + TargetVelocity_0To2*Time + TargetAcceleration_0To2 * Time*Time * 0.5};

	return TargetLocation_Time;
}

FVector UNonLinearProjectileHoming::SimulateProjectileLocation(const FVector UnitDirFromProjectileToTarget, const double Time) const
{
	check(IsValid(ProjectileMovement));

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
	const float OldDeltaTime = DeltaTimeLocationBuffer->GetElement(-2).Key;
	if (OldDeltaTime > 0) // Check if there is there are already two position samples from previous ticks in the buffer
	{
		const FVector OldTargetLocation = DeltaTimeLocationBuffer->GetElement(-1).Value;
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
		GEngine->AddOnScreenDebugMessage(43, GetComponentTickInterval(), FColor::Green,
		                                 FString::Printf(TEXT("TargetLocation"))
		);
		DrawDebugCrosshairs(GetWorld(), TargetLocation, FRotator::ZeroRotator, 200.f,
		                    FColor::Green, false, GetComponentTickInterval()
		);

		GEngine->AddOnScreenDebugMessage(44, GetComponentTickInterval(), FColor::Blue,
		                                 FString::Printf(TEXT("PredictedTargetLocationGuess"))
		);
		DrawDebugCrosshairs(GetWorld(), PredictedTargetLocationGuess, FRotator::ZeroRotator, 200.f,
		                    FColor::Blue, false, GetComponentTickInterval()
		);

		GEngine->AddOnScreenDebugMessage(45, GetComponentTickInterval(), FColor::Red,
		                                 FString::Printf(TEXT("LMPredictedTargetLocation")));
		DrawDebugCrosshairs(GetWorld(), LMPredictedTargetLocation, FRotator::ZeroRotator, 200.f,
		                    FColor::Red, false, GetComponentTickInterval()
		);
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
	check(IsValid(ProjectileMovement));
	check(DeltaTimeLocationBuffer.IsValid());

	const FVector ProjectileLocation = ProjectileMovement->GetOwner()->GetActorLocation();
	const FVector TargetLocation{TargetActor->GetActorLocation()};

	DeltaTimeLocationBuffer->PushNewElement(FDeltaTimeLocationPair{DeltaTime, TargetLocation});

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
