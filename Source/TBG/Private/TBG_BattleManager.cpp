// YangSida Allright reserved


#include "TBG_BattleManager.h"
#include "Character/TBG_Character_ExploreEnemies.h"
#include "Character/TBG_Character_ExplorePlayer.h"

void UTBG_BattleManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UTBG_BattleManager::Deinitialize()
{
	Super::Deinitialize();
}

void UTBG_BattleManager::InitBattle(ATBG_Character_ExploreEnemies* EnemyRef, ATBG_Character_ExplorePlayer* PlayerRef)
{
}
