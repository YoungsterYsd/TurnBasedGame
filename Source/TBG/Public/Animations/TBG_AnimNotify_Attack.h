// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "TBG_ES.h"
#include "TBG_AnimNotify_Attack.generated.h"

/**
 * 
 */
UCLASS()
class TBG_API UTBG_AnimNotify_Attack : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	EAttackType ATKType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	int32 AttackCountInOneCycle = 0;
	
};
