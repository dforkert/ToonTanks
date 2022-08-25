// Copyright Dominik Forkert. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ToonTanksPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TOONTANKS_API AToonTanksPlayerController final : public APlayerController
{
	GENERATED_BODY()

public:
	void SetPlayerEnabledState(bool bPlayerEnabled);
	
};
