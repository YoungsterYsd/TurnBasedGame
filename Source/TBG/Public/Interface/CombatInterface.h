// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TBG_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Int_GetActionValue(float& actionVal) = 0;
	virtual void Int_UpdateActionValue(float winnerVal) = 0;
	virtual void Int_RefreshActionValueBySpeed() = 0;
};
