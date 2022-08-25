// Copyright Dominik Forkert. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BasePawn.h"
#include "Tower.generated.h"

class ATank;

/**
 * 
 */
UCLASS()
class TOONTANKS_API ATower final : public ABasePawn
{
	GENERATED_BODY()

public:
	virtual void Tick(const float DeltaTime) override;

	virtual void DestroyPawn() override;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
private:
	UPROPERTY()
	ATank* Tank;

	
	UPROPERTY(EditDefaultsOnly, Category="Tower Combat")
	float FireRange{500.f};

	UPROPERTY(EditDefaultsOnly, Category="Tower Combat")
	float FireRate{2.f};
	FTimerHandle FireRateTimerHandle;

	/**
	 * @brief Calls Fire function if Tank is in FireRange
	 */
	void CheckFireCondition();

	/**
	 * @brief Checks whether Tank is in FireRange
	 * @return Returns true if Tank exists and is in FireRange; returns false otherwise
	 */
	bool IsTankInFireRange() const;
	
};
