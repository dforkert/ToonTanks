// Copyright Dominik Forkert. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileHomingBase.h"
#include "SimpleLinearProjectileHoming.generated.h"



/**
 * @brief provides a target for homing AProjectile objects; this class implements a naive linear prediction model   
 */
UCLASS(ClassGroup=(Custom))
class TOONTANKS_API USimpleLinearProjectileHoming : public UProjectileHomingBase
{
	GENERATED_BODY()
	
public:
	virtual void UpdateProjectileHomingLocation(
		const float DeltaTime
	) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	using FDeltaTimeLocationPair = TPair<float, FVector>;

	/**
 	* @brief Wrapper class for TCircularBuffer which adds an additional size and position variable
 	* The size variable is not rounded up to the next power of two, therefore, is always less or equal the TCircularBuffer's capacity
 	* @tparam ElementType Type of the elements of the TCircularBuffer 
 	*/
	class FCircularSampleBuffer
	{
	public:
		FCircularSampleBuffer(const uint32 InSampleSize, const FDeltaTimeLocationPair DefaultElement) :
			RequestedSize{InSampleSize},
			CurrentPosition{0},
			Buffer{InSampleSize, DefaultElement} {};

		uint32 GetRequestedSize() const;

		void PushNewElement(FDeltaTimeLocationPair&& Element);

		FDeltaTimeLocationPair GetElement(const int32 OffSet=0) const;

	private:
		const uint32 RequestedSize;
		uint32 CurrentPosition;
		TCircularBuffer<FDeltaTimeLocationPair> Buffer;
	};

	TUniquePtr<FCircularSampleBuffer> PositionBuffer;

	static FVector PredictSimpleLinearTargetLocation(FVector ProjectileVelocity, FVector ProjectileLocation,
														 FVector OldTargetLocation, FVector TargetLocation, float DeltaTime);

	
private:
	/**
	 * @brief Initializes SampleBuffer pointing to an FCircularSampleBuffer object of specified size;
	 */
	virtual void InitializeSampleBuffer();
	
};

