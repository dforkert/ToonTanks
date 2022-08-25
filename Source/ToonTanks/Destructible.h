// Copyright Dominik Forkert. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Destructible.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDestructible : public UInterface
{
	GENERATED_BODY()
};

/**
 * @brief Interface which handles Pawn destruction
 */
class TOONTANKS_API IDestructible
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	/**
	 * @brief Implementations of this pure virtual function should trigger destruction (effects) of Actor when called
	 */
	virtual void DestroyPawn() = 0;
};
