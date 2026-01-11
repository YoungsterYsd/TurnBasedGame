// YangSida Allright reserved


#include "Animations/TBG_AnimNotify_Attack.h"
#include "Interface/CombatInterface.h"

void UTBG_AnimNotify_Attack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ICombatInterface* tempInterface = Cast<ICombatInterface>(MeshComp->GetOwner());
	if (tempInterface)
	{
		tempInterface->Int_SetATK(ATKType, AttackCountInOneCycle);
	}
}
