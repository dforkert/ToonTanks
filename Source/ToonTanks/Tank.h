// Copyright Dominik Forkert. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BasePawn.h"
#include "Tank.generated.h"

class USpringArmComponent;
class UCameraComponent;

/**
 * 
 */
UCLASS()
class TOONTANKS_API ATank final : public ABasePawn
{
	GENERATED_BODY()

public:
	ATank();

	// Called every frame
	virtual void Tick(const float DeltaTime) override;

	virtual void DestroyPawn() override;

	[[nodiscard]] APlayerController* GetTankPlayerController() const
	{
		return TankPlayerController;
	}

	bool bTankAlive{true};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere, Category="Tank-specific Components")
	USpringArmComponent* TankSpringArmComponent;

	UPROPERTY(VisibleAnywhere, Category="Tank-speficic Components")
	UCameraComponent* TankCameraComponent;

	UPROPERTY(EditAnywhere, Category="Tank Movement")
	float Speed{200.f};
	
	UPROPERTY(EditAnywhere, Category="Tank Movement")
	float RotationSpeed{100.f};

	UPROPERTY(VisibleAnywhere)
	APlayerController* TankPlayerController;

	/**
	 * @brief Pushes the Tank orthogonal to the Normal of a HitResult;
	 * the magnitude of the push is determined by the TankMovingVector projected onto the push direction
	 * 
	 * @param MovementHitResult HitResult for the Tank
	 * @param TankMovingVector Vector corresponding to Tank movement in World coordinates
	 */
	void PushTankAwayFromCollision(const FHitResult& MovementHitResult, const FVector& TankMovingVector);

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
	
};
