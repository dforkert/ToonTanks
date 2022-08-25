// Copyright Dominik Forkert. All Rights Reserved.


#include "ToonTanksPlayerController.h"

void AToonTanksPlayerController::SetPlayerEnabledState(const bool bPlayerEnabled)
{
	if (bPlayerEnabled)
	{
		GetPawn()->EnableInput(this);
	}
	else
	{
		GetPawn()->DisableInput(this);
	}
	bShowMouseCursor = bPlayerEnabled;
}
