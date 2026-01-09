// YangSida Allright reserved


#include "Character/TBG_Character_BattleEnemies.h"

void ATBG_Character_BattleEnemies::UpdateLockIcon(bool bHide)
{
}

void ATBG_Character_BattleEnemies::Int_RefreshActionValueBySpeed()
{
	ActionValue = Distance/enemyInfo.Speed;
}

void ATBG_Character_BattleEnemies::BeginPlay()
{
	Super::BeginPlay(); 

	FString s = DataRow.ToString();
	enemyInfo = *(EnemyCharsDT->FindRow<FEnemyCharAttributes>(DataRow, s, true));
	// 初始化行动值
	Int_RefreshActionValueBySpeed();
	//临时初始化UI相关
	bPlayerFaction = false;
	AvatarIcon = enemyInfo.CharIcon_Banner;

}
