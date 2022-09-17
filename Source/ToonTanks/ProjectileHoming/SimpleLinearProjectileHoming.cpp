// Copyright Dominik Forkert. All Rights Reserved.


#include "SimpleLinearProjectileHoming.h"

#include "GameFramework/ProjectileMovementComponent.h"


uint32 USimpleLinearProjectileHoming::FCircularDeltaTimeLocationBuffer::GetCapacity() const
{
	return Buffer.Capacity();
}

void USimpleLinearProjectileHoming::FCircularDeltaTimeLocationBuffer::PushNewElement(FDeltaTimeLocationPair&& Element)
{
	CurrentPosition = Buffer.GetNextIndex(CurrentPosition);
	Buffer[CurrentPosition] = Element;
}

USimpleLinearProjectileHoming::FDeltaTimeLocationPair USimpleLinearProjectileHoming::FCircularDeltaTimeLocationBuffer::GetElement(const int32 OffSet) const
{
	return Buffer[CurrentPosition + OffSet];
}


void USimpleLinearProjectileHoming::BeginPlay()
{
	Super::BeginPlay();
	InitializeSampleBuffer();
}


void USimpleLinearProjectileHoming::InitializeSampleBuffer()
{
	uint32 SampleSize{1};
	FDeltaTimeLocationPair DefaultElement{0.f, FVector::ZeroVector};
	DeltaTimeLocationBuffer = MakeUnique<FCircularDeltaTimeLocationBuffer>(SampleSize, DefaultElement);
}

FVector USimpleLinearProjectileHoming::PredictSimpleLinearTargetLocation(
	const FVector ProjectileVelocity,
	const FVector ProjectileLocation,
	const FVector OldTargetLocation,
	const FVector TargetLocation,
	const float DeltaTime)
{
	const float ProjectileSpeed = ProjectileVelocity.Length();
	float PredictedTimeToImpact{0.f};
	if (ProjectileSpeed > 0.f)
	{
		// Simplifying approximation: distance between Projectile and Target is constant between two consecutive time-steps
		PredictedTimeToImpact = FVector::Dist(ProjectileLocation, TargetLocation) / ProjectileSpeed;
	}

	const FVector PredictedTargetVelocity = (TargetLocation - OldTargetLocation) / DeltaTime;
	return TargetLocation + PredictedTargetVelocity*PredictedTimeToImpact;
}

void USimpleLinearProjectileHoming::UpdateProjectileHomingLocation(const float DeltaTime)
{
	if (ProjectileMovement && DeltaTimeLocationBuffer)
	{
		const FVector TargetLocation{TargetActor->GetActorLocation()};

		
		float PreviousDeltaTime;
		FVector PreviousTargetLocation;
		Tie(PreviousDeltaTime, PreviousTargetLocation) = DeltaTimeLocationBuffer->GetElement();
		
		if (PreviousDeltaTime > 0)	// Check if there is there is already a position sample in the buffer
		{
			const FVector ProjectileLocation{ProjectileMovement->GetOwner()->GetActorLocation()};
			const FVector ProjectileVelocity{ProjectileMovement->Velocity};
			const FVector PredictedTargetLocation{
				PredictSimpleLinearTargetLocation(
					ProjectileVelocity,
					ProjectileLocation,
					PreviousTargetLocation,
					TargetLocation,
					DeltaTime)
			};
			SetWorldLocation(PredictedTargetLocation);
			
			#if WITH_EDITOR
				DrawDebugCrosshairs(GetWorld(), TargetLocation, FRotator::ZeroRotator, 200.f, FColor::Green);
				DrawDebugCrosshairs(GetWorld(), PredictedTargetLocation, FRotator::ZeroRotator, 200.f, FColor::Blue);
			#endif
		}
		
		DeltaTimeLocationBuffer->PushNewElement(FDeltaTimeLocationPair{DeltaTime, TargetLocation});
	}

	
	
};

