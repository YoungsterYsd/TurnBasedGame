 // YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Interface/AnimInterface.h"
#include "AnimPlayerChars.generated.h"

/**
 * 
 */
UCLASS()
class TBG_API UAnimPlayerChars : public UAnimInstance,public IAnimInterface
{
	GENERATED_BODY()
public:
	virtual void Int_SetUltimateReadeVFX(bool bShow) override;
};
 