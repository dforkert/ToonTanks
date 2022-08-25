// Copyright Dominik Forkert. All Rights Reserved.


#include "Tower.h"

#include "Tank.h"
#include "Kismet/GameplayStatics.h"


void ATower::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsTankInFireRange())
		{
			RotateTurret(Tank->GetActorLocation(), DeltaTime);
			
		}
	}

void ATower::DestroyPawn()
{
	Super::DestroyPawn();
	Destroy();
}


void ATower::BeginPlay()
{
	Super::BeginPlay();

	Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));

	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ATower::CheckFireCondition, FireRate, true);
}


/**
 * @brief 
 */
void ATower::CheckFireCondition()
{
	if (Tank && Tank->bTankAlive && IsTankInFireRange())
	{
		Fire();
	}
}


/**
 * @brief 
 * @return 
 */
bool ATower::IsTankInFireRange() const
{
	const float DistanceToTank{
		static_cast<float>(
			FVector::Dist(GetActorLocation(), Tank->GetActorLocation())
		)
	};
	return DistanceToTank <= FireRange;
}
