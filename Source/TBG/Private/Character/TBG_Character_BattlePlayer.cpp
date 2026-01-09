// YangSida Allright reserved


#include "Character/TBG_Character_BattlePlayer.h"


void ATBG_Character_BattlePlayer::Int_RefreshActionValueBySpeed()
{
	ActionValue = Distance/ playerAtr.Speed;
}

void ATBG_Character_BattlePlayer::BeginPlay()
{
	Super::BeginPlay();
	FString s = DataRow.ToString();
	playerAtr = *(PlayerCharsDT->FindRow<FPlayerCharAttributes>(DataRow, s, true));

	// 初始化行动值
	Int_RefreshActionValueBySpeed();

	//初始化UI相关
	bPlayerFaction = true;
	AvatarIcon = playerAtr.CharIcon_Banner;
}
