 // YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Interface/AnimInterface.h"
#include "TBG_ES.h"
#include "AnimPlayerChars.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
UCLASS()
class TBG_API UAnimPlayerChars : public UAnimInstance,public IAnimInterface
{
	GENERATED_BODY()
public:
	//指定为线程安全的类型，否则编译会报警
	UFUNCTION(BlueprintCallable,BlueprintPure, meta = (BlueprintThreadSafe = "true"))
	EAttackType const GetAttackType();

	virtual void Int_SetUltimateReadyVFX(bool bShow) override;
	void SpawnEmitterFunc();
protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds)override;
public:
	EAttackType attackType{ EAttackType::AT_EMAX };

	UNiagaraComponent* NComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Presets")
	UNiagaraSystem* UltimateReadyVFX;
};
 