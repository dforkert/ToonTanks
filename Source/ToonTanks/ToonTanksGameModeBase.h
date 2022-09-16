// Copyright Dominik Forkert. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ToonTanksGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class TOONTANKS_API AToonTanksGameModeBase final : public AGameModeBase
{
	GENERATED_BODY()

public:
	bool IsWinConditionSatisfied() const;

	void ActorTookDamage(AActor* DeadActor, float CurrentHealth, float MaxHealth);
	
	void ActorDied(AActor* DeadActor);

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void PlayerTankTookDamage(float CurrentHealth, float MaxHealth);
	
	UFUNCTION(BlueprintImplementableEvent)
	void StartGame();

	UFUNCTION(BlueprintImplementableEvent)
	void GameOver(bool bIsGameWon);


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Player Pawns", meta=(AllowPrivateAccess="true"))
	class ATank* Tank;
	// ReSharper disable once CppUE4ProbableMemoryIssuesWithUObject
	class AToonTanksPlayerController* ToonTanksPlayerController;

	UPROPERTY(EditDefaultsOnly, Category="Game Start", meta=(AllowPrivateAccess="true"))
	float StartDelay{3.f};

	void HandleGameStart();

	int32 TargetTowersCount{0};
	/**
	 * @brief 
	 * @return 
	 */
	int32 GetTargetTowersCount() const;
};
