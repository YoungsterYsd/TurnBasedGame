// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ExploreInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UExploreInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TBG_API IExploreInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Int_E_ATK(bool bStart) = 0;
};
