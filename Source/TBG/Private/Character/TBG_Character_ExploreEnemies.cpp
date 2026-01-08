// YangSida Allright reserved


#include "Character/TBG_Character_ExploreEnemies.h"
#include "Character/TBG_Character_ExplorePlayer.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "TBG_BattleManager.h"

ATBG_Character_ExploreEnemies::ATBG_Character_ExploreEnemies()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATBG_Character_ExploreEnemies::BeginPlay()
{
	Super::BeginPlay();
}

void ATBG_Character_ExploreEnemies::EnterBattleThroughBM(ATBG_Character_ExplorePlayer* playerRef)
{
	//todo 将玩家和敌人加入战斗管理器
	UTBG_BattleManager* BattleManagerRef = Cast<UTBG_BattleManager>(USubsystemBlueprintLibrary::GetGameInstanceSubsystem(this, UTBG_BattleManager::StaticClass()));
	if (BattleManagerRef == nullptr)return;
	BattleManagerRef->InitBattle(this, playerRef);
}
