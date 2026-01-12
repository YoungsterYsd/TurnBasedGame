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
class USoundBase;
UCLASS(Blueprintable,BlueprintType)
class TBG_API ATBG_BattleManager : public AActor
{
	GENERATED_BODY()
public:
	ATBG_BattleManager();
	void InitBattle(ATBG_Character_ExploreEnemies* EnemyRef, ATBG_Character_ExplorePlayer* PlayerRef);
	
	void PreInitializeBattle();
	void PostInitialzeBattle();
	void BattleEnd(EBattleFlags endResult);
	void HandlePlayerAttack(ATBG_Character_BattlePlayer* InPlayer);
	void HandleEnemyAttack(ATBG_Character_BattleEnemies* InEnemy);
	void TurnEnd(AActor* endTurnActor,bool bConsumeTurn);

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
	void CalculateActionValue_EP();
	EBattleFlags CheckGameOver(TMap<ATBG_Character_BattleEnemies*,float> eArr, TMap<ATBG_Character_BattlePlayer*, float> pArr);

	//我方攻击
	void SwitchEnemyLockIcon(bool bNext);
	void SetMultipleEnemyLocks();

	//Utilities
	bool IsMutipleTargets();
	bool NotResurrectSkill();
	bool IsBuffTarget();
	bool IsMeleeAction();

	//Locks
	void ShowEnemyLockIconByIndex(int32 Index);
	void ShowEnemyLockIconMultiple(TArray<ATBG_Character_BattleEnemies*>  InCurrentEnemyTargets);
	void ShowPlayerLockIconByIndex(int32 Index);
	void ShowPlayerLockIconMultiple(TArray<ATBG_Character_BattlePlayer*>  InCurrentPlayerTargets);
	void CalculateLockIndex(bool bNext);
	void SetDeadPlayerLockedIcons();
	void SetPlayerLockedIcons();
	void HideAllLockedIcons();
	void DisplayLockedIconsAndSetTargets();
	void UpdatePlayerLockedIconToMultiple();
	void UpdateEnemyLockedIconToMultiple();
	void SwitchAndHideOtherPlayerChars(bool bHideOther,ATBG_Character_BattlePlayer* activePlayer);

	//ActionAttck
	void ExecuteAction(EAttackType ATKType);
	void HandlePlayerATK(EAttackType ATKType);
	void ExecuteUltimate();
	void EnterUltimate(int32 playerSPIndex);

	void ResetActionValueAndATKType(bool bCounsumeTurn, AActor* endActor);
	void ReadyForUltimate(ATBG_Character_BattlePlayer* readyCharRef);
	void RemoveUltimateTurn(AActor* charRef);

	void CheckPlayerRevive();

	//Camera
	void CameraForBuffSelections();

	//EnemyDeath
	UFUNCTION()
	void EnemyDeath(ATBG_Character_BattleEnemies* enemyRef, AActor* causerRef);
	UFUNCTION()
	void EnemyTurnEnd(ATBG_Character_BattleEnemies* enemyRef);

	//Battle Handle;
	UFUNCTION()
	void HandleDelays();
	UFUNCTION()
	void BattleEndCameraStartingFade();
	UFUNCTION()
	void CleanBattleField();
	void PlayerWin();
	void EnemyWin();
	void ExitGame();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime)override;
	void CameraForBuffSmooth(float DeltaTime, bool& retFlag);
	EProgressPhase ProgressPhase{ EProgressPhase::PP_EMAX}; 
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	TSubclassOf<ATBG_BattlePawn> BattlePawnClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	TSubclassOf<UUserWidget> BattleLayoutClassRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	TMap<int32, ATBG_Character_BattlePlayer*> TeamInstForUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	float EnemyDisplayTime = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	USoundBase* UnableSFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	USoundBase* FailedSFX;

	UBattleLayOut* BattleLayOut;
	EBattleFlags CurBattleFlag{ EBattleFlags::BF_EMAX };
	void LoadBattleUI();

	ATBG_Character_ExploreEnemies* ExploreEnemyRef = nullptr;
	ATBG_Character_ExplorePlayer* ExplorePlayerRef = nullptr;
	TMap<int32, TSubclassOf<ATBG_Character_BattleEnemies>> EnemyTeamInfo;
	TMap<int32, TSubclassOf<ATBG_Character_BattlePlayer>> PlayerTeamInfo;

	TArray<ATBG_Character_BattleEnemies*> enemiesRefArr;
	TArray<ATBG_Character_BattleEnemies*> deadEnemyRefArr;

	TArray<ATBG_Character_BattlePlayer*> playerRefArr;
	TArray<ATBG_Character_BattlePlayer*> deadPlayerRefArr;
	TArray<ATBG_Character_BattlePlayer*> ultimatePlayerQueue;

	TArray<ACameraActor*> camerasArr;
	TArray<AActor*> enemySpawnPointsArr;
	TArray<AActor*> playerSpawnPointsArr;

	ATBG_BattlePawn* BattlePawn;

	FTimerHandle DisplayEnemyTimerHandle;
	FTimerHandle HandleDelaysTimerHandle;
	FTimerHandle BattleEndCameraStartingFadeTimerHandle;
	FTimerHandle CleanBattleFieldTimerHandle;
	FTimerHandle ExitGameTimerHandle;
	//处理活跃中的角色
	ATBG_Character_BattleEnemies* ActiveEnemy;
	ATBG_Character_BattlePlayer* ActivePlayer;

	//选中对象相关
	ATBG_Character_BattleEnemies*			currentEnemyTarget; 
	TArray<ATBG_Character_BattleEnemies*>	currentEnemyTargets;
	ATBG_Character_BattlePlayer*			currentPlayerTarget;
	TArray<ATBG_Character_BattlePlayer*>	currentPlayerTargets;
	//鼠标点击用
	AActor* lastClickedActor;
	ACameraActor* buffCamera;
	FVector buffCameraOriginLocation;

	int32 indexForLockedTarget = 2;
	int32 skillPoints = 2;//初始技能点，普工增加

	//TOdo 需要在返回普通状态时，将是否boss战变量置为false
	bool bBOSSFight = false;

	FString normalCA = "tag_c_start_normal";
	FString fixedCA = "tag_c_boss";
	FString buffCA = "tag_c_buff";
};
