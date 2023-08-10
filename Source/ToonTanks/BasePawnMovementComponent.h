// Copyright Dominik Forkert. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BasePawnMovementComponent.generated.h"

class AHollowSphere;

UENUM()
enum class EMovementGeometry
{
	Euclidean,
	Spherical
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TOONTANKS_API UBasePawnMovementComponent : public USceneComponent
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* Owner;

	UPROPERTY(EditInstanceOnly, Category="Tank Movement")
	EMovementGeometry MovementGeometry{EMovementGeometry::Euclidean};
	
	UPROPERTY(EditAnywhere, Category="Tank Movement")
	float Speed{200.f};
	
	UPROPERTY(EditAnywhere, Category="Tank Movement")
	float RotationSpeed{100.f};

	UPROPERTY(VisibleInstanceOnly)
	AHollowSphere* HollowSphere;
	
public:
	// Sets default values for this component's properties
	UBasePawnMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	/**
 	* @brief Moves the Tank along its ForwardVector according to the Controller AxisValue
 	* @param ControllerAxisValue A positive values moves the Tank forwards; a negative value moves the Tank backwards
 	*/
	void Move(const float ControllerAxisValue);
	
	/**
	 * @brief Rotates the Tank according to the Controller AxisValue
	 * @param ControllerAxisValue A positive values rotates the Tank to the left; a negative value rotates the Tank to the right
	 */
	void Turn(const float ControllerAxisValue);

	/**
	 * @brief Sets the MovementGeometry for this movement component
	 * @param NewMovementGeometry New MovementGeometry for this movement component
	 * @param AssociatedHollowSphere HollowSphere associated with spherical movement; otherwise nullptr
	 */
	void SetMovementGeometry(const EMovementGeometry NewMovementGeometry, AHollowSphere* AssociatedHollowSphere = nullptr);

private:
	/**
	 * @brief Pushes the Tank orthogonal to the Normal of a HitResult;
	 * the magnitude of the push is determined by the TankMovingVector projected onto the push direction
	 * 
	 * @param MovementHitResult HitResult for the Tank
	 * @param LocalDeltaMovement Vector corresponding to Tank movement in local coordinates
	 */
	void PushAwayFromCollision(const FHitResult& MovementHitResult, const FVector& LocalDeltaMovement) const;

	/**
	 * @brief Moves the BasePawn inside a HollowSphere
	 * @param ControllerAxisValue A positive values moves the Tank forwards; a negative value moves the Tank backwards
	 * @param DeltaTime 
	 */
	void MoveInHollowSphere(const float ControllerAxisValue, const float DeltaTime) const;

	/**
	 * @brief Moves the BasePawn in an Euclidean plane
	 * @param ControllerAxisValue A positive values moves the Tank forwards; a negative value moves the Tank backwards
	 * @param DeltaTime 
	 */
	void MoveInPlane(const float ControllerAxisValue, const float DeltaTime) const;
};
