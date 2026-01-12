// YangSida Allright reserved


#include "Animations/ANS_ExploreerATK.h"
#include "Interface/ExploreInterface.h"

void UANS_ExploreerATK::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	//Super::NotifyBegin()
	IExploreInterface* tempInterface = Cast<IExploreInterface>(MeshComp->GetOwner());
	if (tempInterface)
	{
		tempInterface->Int_E_ATK(true);
	}
}

void UANS_ExploreerATK::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	IExploreInterface* tempInterface = Cast<IExploreInterface>(MeshComp->GetOwner());
	if (tempInterface)
	{
		tempInterface->Int_E_ATK(false);
	}
}
