// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "TBG_ES.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TBG_BattleManager.generated.h"

/**
 * 
 */
 //参考教程使用自定义单例进行制作，但UE提供更便捷的Subsystem系统，更换实现方式。
class ATBG_Character_ExploreEnemies;
class ATBG_Character_ExplorePlayer;
class ATBG_Character_BattlePlayer;
class ATBG_Character_BattleEnemies;
class ATBG_BattlePawn;
class ACameraActor;

UCLASS()
class TBG_API UTBG_BattleManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }
	virtual void Initialize(FSubsystemCollectionBase& Collection)override;
	virtual void Deinitialize()override;
	void InitBattle(ATBG_Character_ExploreEnemies* EnemyRef, ATBG_Character_ExplorePlayer* PlayerRef);
	
	void PreInitializeBattle();
	void ChangeCameraAndStopMovement();

	ACameraActor* RetrieveCamera(FName tag);

protected:

	EProgressPhase ProgressPhase{ EProgressPhase::PP_EMAX};
public:

	ATBG_Character_ExploreEnemies* ExploreEnemyRef = nullptr;
	ATBG_Character_ExplorePlayer* ExplorePlayerRef = nullptr;
	TMap<int32, TSubclassOf<ATBG_Character_BattleEnemies>> EnemyTeamInfo;
	TMap<int32, TSubclassOf<ATBG_Character_BattlePlayer>> PlayerTeamInfo;
	
	TArray<ATBG_Character_BattleEnemies*> ememiesRefArr;
	TArray<ATBG_Character_BattleEnemies*> deadEnemyRefArr;

	TArray<ATBG_Character_ExplorePlayer*> playerRefArr;
	TArray<ATBG_Character_ExplorePlayer*> deadPlayerRefArr;

	TArray<ACameraActor*> camerasArr;
	TArray<AActor*> enemySpawnPointsArr;
	TArray<AActor*> playerSpawnPointsArr;

	ATBG_BattlePawn* BattlePawn;
private:
	//TOdo 需要在返回普通状态时，将是否boss战变量置为false
	bool bBOSSFight = false;
};
