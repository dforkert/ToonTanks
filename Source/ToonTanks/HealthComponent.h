// Copyright Dominik Forkert. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class ABasePawn;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TOONTANKS_API UHealthComponent final : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	float MaxHealth{100.f};
	float Health{0.f};

	UFUNCTION()
	void TakeDamage(
		AActor* ActorToBeDamaged,
		float DamageAmount,
		const UDamageType* DamageType,
		AController* Instigator,
		AActor* DamageCauser
		);


	/**
	 * @brief Triggers when the Owner of the HealthComponent is damaged
	 */
	void DamageOwner() const;

	/**
	 * @brief Triggers when the Owner of the HealthComponent is killed
	 */
	void KillOwner() const;
	
	// ReSharper disable once CppUE4ProbableMemoryIssuesWithUObject
	class AToonTanksGameModeBase* ToonTanksGameModeBase;

public:	
	// Called every frame
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
		) override;

		
};
