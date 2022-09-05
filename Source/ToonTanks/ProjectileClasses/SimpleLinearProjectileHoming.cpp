// Copyright Dominik Forkert. All Rights Reserved.


#include "SimpleLinearProjectileHoming.h"

#include "GameFramework/ProjectileMovementComponent.h"


void USimpleLinearProjectileHoming::BeginPlay()
{
	Super::BeginPlay();
	InitializeSampleBuffer();
}


template <typename ElementType>
uint32 USimpleLinearProjectileHoming::TCircularSampleBuffer<ElementType>::GetRequestedSize() const
{
	return RequestedSize;
}

template <typename ElementType>
void USimpleLinearProjectileHoming::TCircularSampleBuffer<ElementType>::PushNewElement(ElementType Element)
{
	CurrentPosition = Buffer.GetNextIndex(CurrentPosition);
	Buffer[CurrentPosition] = Element;
}

template <typename ElementType>
ElementType USimpleLinearProjectileHoming::TCircularSampleBuffer<ElementType>::GetElement(const int32 OffSet)
{
	return Buffer[CurrentPosition + OffSet];
}



void USimpleLinearProjectileHoming::InitializeSampleBuffer()
{
	uint32 SampleSize{1};
	FDeltaTimeLocationPair DefaultElement{0.f, FVector::ZeroVector};
	PositionBuffer = MakeUnique<TCircularSampleBuffer<FDeltaTimeLocationPair>>(SampleSize, DefaultElement);
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

void USimpleLinearProjectileHoming::UpdateProjectileHomingLocation(const FVector TargetLocation, UProjectileMovementComponent const* ProjectileMovement, const float DeltaTime)
{
	if (ProjectileMovement && PositionBuffer)
	{
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
				
			SetWorldLocation(PredictedTargetLocation);
			
			DrawDebugDirectionalArrow(GetWorld(), TargetLocation, PredictedTargetLocation, 100.f, FColor::Red);
			DrawDebugCrosshairs(GetWorld(), OldTargetLocation, FRotator::ZeroRotator, 300.f, FColor::Green);
			DrawDebugCrosshairs(GetWorld(), PredictedTargetLocation, FRotator::ZeroRotator, 300.f, FColor::Red);
		}
		
		PositionBuffer->PushNewElement(FDeltaTimeLocationPair{DeltaTime, TargetLocation});
	}

	
	
};

