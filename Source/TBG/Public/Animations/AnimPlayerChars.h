 // YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Interface/AnimInterface.h"
#include "TBG_ES.h"
#include "AnimPlayerChars.generated.h"

/**
 * 
 */
UCLASS()
class TBG_API UAnimPlayerChars : public UAnimInstance,public IAnimInterface
{
	GENERATED_BODY()
public:
	//指定为线程安全的类型，否则编译会报警
	UFUNCTION(BlueprintCallable,BlueprintPure, meta = (BlueprintThreadSafe = "true"))
	EAttackType const GetAttackType();

	virtual void Int_SetUltimateReadeVFX(bool bShow) override;
protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds)override;
public:
	EAttackType attackType;
};
 