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
class ATBG_CharacterBase_Battle;
class UBattleLayOut;
class UUserWidget;
UCLASS(Blueprintable)
class TBG_API UTBG_BattleManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }
	virtual void Initialize(FSubsystemCollectionBase& Collection)override;
	virtual void Deinitialize()override;
	void InitBattle(ATBG_Character_ExploreEnemies* EnemyRef, ATBG_Character_ExplorePlayer* PlayerRef);
	
	void PreInitializeBattle();
	void PostInitialzeBattle();

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
	

protected:

	EProgressPhase ProgressPhase{ EProgressPhase::PP_EMAX};
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	TSubclassOf<ATBG_BattlePawn> BattlePawnClass;
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

private:
	//TOdo 需要在返回普通状态时，将是否boss战变量置为false
	bool bBOSSFight = false;
	TArray<ACharacter*> local_SortedCharacters;
};
