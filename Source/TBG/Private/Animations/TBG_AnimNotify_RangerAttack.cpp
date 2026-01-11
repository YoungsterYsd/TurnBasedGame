// YangSida Allright reserved


#include "Animations/TBG_AnimNotify_RangerAttack.h"
#include "Interface/VFXInterface.h"

void UTBG_AnimNotify_RangerAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	IVFXInterface* tempInterface = Cast<IVFXInterface>(MeshComp->GetOwner());
	if (tempInterface)
	{
		tempInterface->Int_SetArrowVFX(bRain);
	}
}
