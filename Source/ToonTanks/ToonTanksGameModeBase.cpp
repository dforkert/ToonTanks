// Copyright Dominik Forkert. All Rights Reserved.


#include "ToonTanksGameModeBase.h"

#include "Tank.h"
#include "ToonTanksPlayerController.h"
#include "Tower.h"
#include "Kismet/GameplayStatics.h"

void AToonTanksGameModeBase::ActorDied(AActor* DeadActor)
{
	if (ABasePawn* DeadBasePawn{Cast<ABasePawn>(DeadActor)})
	{
		DeadBasePawn->HandleDestruction();

		if (DeadBasePawn == Tank)
		{
			if (ToonTanksPlayerController)
			{
				ToonTanksPlayerController->SetPlayerEnabledState(false);
			}
			GameOver(false);
		}
		else if (Cast<ATower>(DeadBasePawn))
		{
			TargetTowersCount--;
			if (TargetTowersCount <= 0)
			{
				GameOver(true);
			}
		}
	}
}


void AToonTanksGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	HandleGameStart();
}

/**
 * @brief 
 */
void AToonTanksGameModeBase::HandleGameStart()
{
	TargetTowersCount = GetTargetTowersCount();
	Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));
	ToonTanksPlayerController = Cast<AToonTanksPlayerController>(
		UGameplayStatics::GetPlayerController(this, 0)
		);

	StartGame(); //BlueprintImplementableEvent

	if (ToonTanksPlayerController)
	{
		ToonTanksPlayerController->SetPlayerEnabledState(false);
		
		FTimerHandle PlayerEnableTimerHandle;
		const FTimerDelegate PlayerEnableTimerDelegate{
			FTimerDelegate::CreateUObject(
				ToonTanksPlayerController,
				&AToonTanksPlayerController::SetPlayerEnabledState,
				true
				)
		};
		GetWorldTimerManager().SetTimer(
			PlayerEnableTimerHandle,
			PlayerEnableTimerDelegate,
			StartDelay,
			false
			);
	}
	
}

int32 AToonTanksGameModeBase::GetTargetTowersCount() const
{
	TArray<AActor*> Towers;
	UGameplayStatics::GetAllActorsOfClass(this, ATower::StaticClass(), Towers);
	return Towers.Num();
}
