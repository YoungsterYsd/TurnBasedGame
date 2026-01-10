// YangSida Allright reserved


#include "GameplayFrame/TBG_BattlePawn.h"
#include "Utilities/CF_SR.h"
#include "TBG_BattleManager.h"
#include "GameplayFrame/TBG_PlayerControllerBase.h"


ATBG_BattlePawn::ATBG_BattlePawn()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ATBG_BattlePawn::TryAction(EAttackType ATKType)
{
	UCF_SR::Flib_GetBM()->ExecuteAction(ATKType);
}

void ATBG_BattlePawn::TrySwitchingEnemyLockIcon(bool bNext)
{
	UCF_SR::Flib_GetBM()->SwitchEnemyLockIcon(bNext);
}


void ATBG_BattlePawn::BeginPlay()
{
	Super::BeginPlay();
	//显示鼠标，改变游戏模式
	PC = Cast<ATBG_PlayerControllerBase>(Controller);
	if (PC == nullptr)return;
	PC->bShowMouseCursor = true;
	PC->SetInputMode(FInputModeGameAndUI());
}

void ATBG_BattlePawn::Destroyed()
{
	Super::Destroyed();

	PC = Cast<ATBG_PlayerControllerBase>(Controller);
	if (PC == nullptr)return;
	PC->bShowMouseCursor = false;
	PC->SetInputMode(FInputModeGameOnly());
}



