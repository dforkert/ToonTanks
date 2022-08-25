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

	virtual void HandleDestruction() override;

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

	void Move(const float Value);
	void Turn(const float Value);

	UPROPERTY(VisibleAnywhere)
	APlayerController* TankPlayerController;
	
};
