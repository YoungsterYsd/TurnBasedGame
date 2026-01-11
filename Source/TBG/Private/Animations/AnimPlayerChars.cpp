// YangSida Allright reserved


#include "Animations/AnimPlayerChars.h"
#include "Character/TBG_Character_BattlePlayer.h"

EAttackType const UAnimPlayerChars::GetAttackType()
{
	return attackType;
}

void UAnimPlayerChars::Int_SetUltimateReadeVFX(bool bShow)
{
	//TBD_大招可用时测试
}

void UAnimPlayerChars::NativeUpdateAnimation(float DeltaSeconds)
{
	ATBG_Character_BattlePlayer* tempPlayerRef = Cast<ATBG_Character_BattlePlayer>(TryGetPawnOwner());
	if (tempPlayerRef)
	{
		attackType = tempPlayerRef->attackType;
	}
}
