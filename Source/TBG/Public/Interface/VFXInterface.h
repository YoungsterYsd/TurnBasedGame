// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VFXInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UVFXInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TBG_API IVFXInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Int_SetArrowVFX(bool bRain) = 0;
};
