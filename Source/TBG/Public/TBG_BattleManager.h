// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "TBG_ES.h"
#include "GameFramework/Actor.h"
#include "TBG_BattleManager.generated.h"

/**
 * 
 */
 //参考教程使用自定义单例进行制作，更换实现方式。
class ATBG_Character_ExploreEnemies;
class ATBG_Character_ExplorePlayer;
class ATBG_Character_BattlePlayer;
class ATBG_Character_BattleEnemies;
class ATBG_BattlePawn;
class ACameraActor;
class ATBG_CharacterBase_Battle;
class UBattleLayOut;
class UUserWidget;
UCLASS(Blueprintable,BlueprintType)
class TBG_API ATBG_BattleManager : public AActor
{
	GENERATED_BODY()
public:
	void InitBattle(ATBG_Character_ExploreEnemies* EnemyRef, ATBG_Character_ExplorePlayer* PlayerRef);
	
	void PreInitializeBattle();
	void PostInitialzeBattle();
	void BattleEnd(EBattleFlags endResult);
	void HandlePlayerAttack(ATBG_Character_BattlePlayer* InPlayer);
	void HandleEnemyAttack(ATBG_Character_BattleEnemies* InEnemy);

	//A1
	void ChangeCameraAndStopMovement();
	ACameraActor* RetrieveCamera(FName tag);
	void InitSpawnPostion();
	void RetrieveEnemyPosition(int32 PosIndex, FVector& TargetPos, float& yaw);
	void RetrievePlayerPosition(int32 PosIndex, FVector& TargetPos, float& yaw);
	void SpawnEnemiesAndDecideLocation();
	void SpawnPlayerAndDecideLocation();

	//B1
	void CalculateActionValue();
	EBattleFlags CheckGameOver(TMap<ATBG_Character_BattleEnemies*,float> eArr, TMap<ATBG_Character_BattlePlayer*, float> pArr);

	//我方攻击
	void SwitchEnemyLockIcon(bool bNext);
	void SetMultipleEnemyLocks();

	//Utilities
	bool IsMutipleTargets();
	bool NotResurrectSkill();
	bool IsBuffTarget();

	//Locks
	void ShowEnemyLockIconByIndex(int32 Index);
	void ShowEnemyLockIconMultiple(TArray<ATBG_Character_BattleEnemies*>  InCurrentEnemyTargets);
	void CalculateLockIndex(bool bNext);
	void SetDeadplayerLockedIcons();
	void SetPlayerLockedIcons();
protected:
	virtual void BeginPlay() override;
	EProgressPhase ProgressPhase{ EProgressPhase::PP_EMAX}; 
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	TSubclassOf<ATBG_BattlePawn> BattlePawnClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	TSubclassOf<UUserWidget> BattleLayoutClassRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	TMap<int32, ATBG_CharacterBase_Battle*> TeamInstForUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	float EnemyDisplayTime = 0.2f;

	UBattleLayOut* BattleLayOut;
	void LoadBattleUI();

	ATBG_Character_ExploreEnemies* ExploreEnemyRef = nullptr;
	ATBG_Character_ExplorePlayer* ExplorePlayerRef = nullptr;
	TMap<int32, TSubclassOf<ATBG_Character_BattleEnemies>> EnemyTeamInfo;
	TMap<int32, TSubclassOf<ATBG_Character_BattlePlayer>> PlayerTeamInfo;

	TArray<ATBG_Character_BattleEnemies*> enemiesRefArr;
	TArray<ATBG_Character_BattleEnemies*> deadEnemyRefArr;

	TArray<ATBG_Character_BattlePlayer*> playerRefArr;
	TArray<ATBG_Character_BattlePlayer*> deadPlayerRefArr;

	TArray<ACameraActor*> camerasArr;
	TArray<AActor*> enemySpawnPointsArr;
	TArray<AActor*> playerSpawnPointsArr;

	ATBG_BattlePawn* BattlePawn;
	FTimerHandle DisplayEnemyTimeHandle;
	//处理活跃中的角色
	ATBG_Character_BattleEnemies* ActiveEnemy;
	ATBG_Character_BattlePlayer* ActivePlayer;

	//选中的敌人相关
	ATBG_Character_BattleEnemies* currentEnemyTarget; 
	TArray<ATBG_Character_BattleEnemies*>  currentEnemyTargets;
	//鼠标点击用
	AActor* lastClickedEnemyActor;
	int32 indexForLockedTarget = 2;
private:
	//TOdo 需要在返回普通状态时，将是否boss战变量置为false
	bool bBOSSFight = false;
};
