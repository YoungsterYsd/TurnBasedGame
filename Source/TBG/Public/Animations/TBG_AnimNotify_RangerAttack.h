// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "TBG_AnimNotify_RangerAttack.generated.h"

/**
 * 
 */
UCLASS()
class TBG_API UTBG_AnimNotify_RangerAttack : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	bool bRain = false;
	
};
