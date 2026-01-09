// YangSida Allright reserved


#include "GameplayFrame/TBG_PlayerControllerBase.h"
#include "BattleLayOut.h"
#include "TBG_BattleManager.h"

void ATBG_PlayerControllerBase::LoadBattleUI(UTBG_BattleManager* InBM)
{
	if (BattleLayoutClassRef)
	{
		BattleLayOut = CreateWidget<UBattleLayOut>(GetWorld(), BattleLayoutClassRef);
		if (BattleLayOut)
		{
			BattleLayOut->ConstructDeferred(InBM);
			BattleLayOut->AddToViewport();
		}
	}
}
