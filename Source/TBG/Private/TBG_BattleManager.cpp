// YangSida Allright reserved


#include "TBG_BattleManager.h"
#include "Character/TBG_Character_ExploreEnemies.h"
#include "Character/TBG_Character_ExplorePlayer.h"
#include "Camera/CameraActor.h"
#include "GameplayFrame/TBG_BattlePawn.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayFrame/TBG_PlayerControllerBase.h"
#include "Character/TBG_Character_BattlePlayer.h"
#include "Character/TBG_Character_BattleEnemies.h"
#include "BattleLayOut.h"
#include "Interface/CombatInterface.h"
#include "Kismet\KismetMathLibrary.h"

void UTBG_BattleManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UTBG_BattleManager::Deinitialize()
{
	Super::Deinitialize();
}

void UTBG_BattleManager::InitBattle(ATBG_Character_ExploreEnemies* InEnemyRef, ATBG_Character_ExplorePlayer* InPlayerRef)
{
	ExploreEnemyRef = InEnemyRef;
	ExplorePlayerRef = InPlayerRef;
	EnemyTeamInfo = InEnemyRef->EnemyTeamInfo;
	PlayerTeamInfo = InPlayerRef->PlayerTeamInfo;
	bBOSSFight = ExploreEnemyRef->bBOSSFight;
	//初始化战场
	PreInitializeBattle();
	//展示敌人阵容的时间
	GetWorld()->GetTimerManager().SetTimer(DisplayEnemyTimeHandle, this,&UTBG_BattleManager::PostInitialzeBattle,EnemyDisplayTime,false);

}

void UTBG_BattleManager::PreInitializeBattle()
{

	//改变战斗阶段
	ProgressPhase = EProgressPhase::PP_A1_PreInitialization;
	//清理旧缓存
	enemiesRefArr.Empty();
	deadEnemyRefArr.Empty();
	playerRefArr.Empty();
	deadPlayerRefArr.Empty();
	//获取战场生成地点信息
	InitSpawnPostion();
	//相机控制
	ChangeCameraAndStopMovement();
	//根据敌人站位信息生成敌人
	SpawnEnemiesAndDecideLocation();
	// 根据玩家队伍信息生成玩家
	SpawnPlayerAndDecideLocation();
	//生成战斗UI/隐藏探索UI
	LoadBattleUI();
	// TBD - 若探索模式下玩家角色被敌人击中，则敌人先手攻击

	// TBD - 进入B1b阶段，计算行动值
}

void UTBG_BattleManager::PostInitialzeBattle()
{
	//改变阶段
	ProgressPhase = EProgressPhase::PP_B1_CalculateActionValue;
	// TBD - 若探索模式下玩家角色被敌人击中，则敌人先手攻击

	// TBD - 进入B1b阶段，计算行动值
	CalculateActionValue();
}

void UTBG_BattleManager::ChangeCameraAndStopMovement()
{
	// 生成Battle Pawn，控制它并使用在其中定义的按键
	BattlePawn = Cast<ATBG_BattlePawn>(
		GetWorld()->SpawnActor<AActor>(BattlePawnClass, FVector(0, 0, 0), FRotator(0, 0, 0)));

	GetWorld()->GetFirstPlayerController()->Possess(BattlePawn);

	// 探索玩家角色停止奔跑，不可移动
	ExploreEnemyRef->GetCharacterMovement()->SetMovementMode(MOVE_None);

	// 改变摄像机视角
	ACameraActor* targetCA;
	FString normalCA = "tag_c_start_normal";
	FString fixedCA = "tag_c_boss";
	if (bBOSSFight)
	{
		targetCA = RetrieveCamera(FName(*fixedCA));
	}
	else
	{
		targetCA = RetrieveCamera(FName(*normalCA));
	} 

	if (targetCA == nullptr) return;
	ATBG_PlayerControllerBase* PC =
	Cast<ATBG_PlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC == nullptr) return;
	PC->SetViewTargetWithBlend(targetCA);

}
ACameraActor* UTBG_BattleManager::RetrieveCamera(FName tag)
{
	// 根据标签寻找对应摄像机
	for (auto ArrayElem : camerasArr)
	{
		if (ArrayElem->ActorHasTag(tag))
		{
			return ArrayElem;
		}
	}

	return nullptr;
}

void UTBG_BattleManager::InitSpawnPostion()
{
	//生成battlePawn，使用其中自定义的按键
	TArray<AActor*> tempActors;
	// 获取场景中所有CameraActors
	UGameplayStatics::GetAllActorsOfClass(GEngine->GameViewport->GetWorld(), ACameraActor::StaticClass(), tempActors);
	for (auto ArrayElem : tempActors)
	{
		camerasArr.Add(Cast<ACameraActor>(ArrayElem));
	}

	// 获取敌人的位置
	tempActors.Empty();
	UGameplayStatics::GetAllActorsOfClass(GEngine->GameViewport->GetWorld(), ATargetPoint::StaticClass(), tempActors);
	for (auto ArrayElem : tempActors)
	{
		if (ArrayElem->ActorHasTag("EnemyPos"))
		{
			enemySpawnPointsArr.Add(ArrayElem);
		}
	}

	// 获取玩家角色的位置
	tempActors.Empty();
	UGameplayStatics::GetAllActorsOfClass(GEngine->GameViewport->GetWorld(), ATargetPoint::StaticClass(), tempActors);
	for (auto ArrayElem : tempActors)
	{
		if (ArrayElem->ActorHasTag("PlayerPos"))
		{
			playerSpawnPointsArr.Add(ArrayElem);
		}
	}	
}

void UTBG_BattleManager::RetrieveEnemyPosition(int32 PosIndex, FVector& TargetPos, float& yaw)
{

	for (auto ArrayElem : enemySpawnPointsArr)
	{
		FString targetTag;
		targetTag = "e" + FString::FromInt(PosIndex);
		bool bTarget = ArrayElem->ActorHasTag(*targetTag); // FName
		if (bTarget)
		{
			TargetPos = ArrayElem->GetActorLocation();
			yaw = ArrayElem->GetActorRotation().Yaw;
			return; // 立刻跳出，重要
		}
	}
	TargetPos = FVector(0, 0, 0);
	yaw = 0.0f;
	return;
}
void UTBG_BattleManager::RetrievePlayerPosition(int32 PosIndex, FVector& TargetPos, float& yaw)
{

	for (auto ArrayElem : playerSpawnPointsArr)
	{
		FString targetTag;
		targetTag = "p" + FString::FromInt(PosIndex);
		bool bTarget = ArrayElem->ActorHasTag(*targetTag); // FName
		if (bTarget)
		{
			TargetPos = ArrayElem->GetActorLocation();
			yaw = ArrayElem->GetActorRotation().Yaw;
			return; // 立刻跳出，重要
		}
	}
	TargetPos = FVector(0, 0, 0);
	yaw = 0.0f;
	return;
}
void UTBG_BattleManager::SpawnEnemiesAndDecideLocation()
{
	for (auto It = EnemyTeamInfo.CreateConstIterator(); It; ++It)
	{
		// 按照敌人的类别ID生成战斗对象
		FVector Location;
		FRotator Rotation;
		float yaw;
		RetrieveEnemyPosition(It.Key(), Location, yaw);
		Rotation = FRotator(0, yaw, 0);
		TSubclassOf<ATBG_Character_BattleEnemies> tempEnemyClass = *EnemyTeamInfo.Find(It.Key());
		ATBG_Character_BattleEnemies* enemyTemp = GetWorld()->SpawnActor<ATBG_Character_BattleEnemies>(tempEnemyClass, Location, Rotation);
		enemiesRefArr.AddUnique(enemyTemp); // 进入该数组

		// TBD - 绑定敌人回合结束后的回调

		// TBD - 绑定敌人被击败后的回调
	}
}
void UTBG_BattleManager::SpawnPlayerAndDecideLocation()
{
	TeamInstForUI.Empty();
	for (auto It = PlayerTeamInfo.CreateConstIterator(); It; ++It)
	{
		// 按照player的类别ID生成战斗对象
		FVector Location;
		FRotator Rotation;
		float yaw;
		RetrievePlayerPosition(It.Key(), Location, yaw);
		Rotation = FRotator(0, yaw, 0);
		TSubclassOf<ATBG_Character_BattlePlayer> tempPlayerClass = *PlayerTeamInfo.Find(It.Key());
		//由于玩家的编队站位会有变化，故需要延后生成

		FTransform Transform = FTransform(Rotation, Location, FVector(1, 1, 1));
		if(tempPlayerClass->IsValidLowLevel()&& tempPlayerClass)
		{ 
			ATBG_Character_BattlePlayer* playerTemp = GetWorld()->SpawnActorDeferred<ATBG_Character_BattlePlayer>(tempPlayerClass, Transform);
			if (playerTemp)
			{
				playerTemp->PositionID = It.Key();
				playerTemp->FinishSpawning(Transform);
				playerRefArr.AddUnique(playerTemp); // 进入该数组
				TeamInstForUI.Add(It.Key(), playerTemp);
			}
		}

		// TBD - 绑定player回合结束后的回调

		// TBD - 绑定player被击败后的回调
	}
}

void UTBG_BattleManager::CalculateActionValue()
{

	//定义排序相关本地变量
	TMap< ATBG_Character_BattleEnemies*, float> local_Enemy_ActionValue;
	TMap< ATBG_Character_BattlePlayer*, float> local_Player_ActionValue;
	TMap<ACharacter*, float>local_CharacterQueue;
	local_SortedCharacters.Empty();
	float local_WinnerActionVal = 0.f;//所有成员需要减去该行动值
	// 检查角色是否存活，获取最新行动值剔除不可移动对象
	for (auto ArrElement : enemiesRefArr)
	{
		if (!ArrElement->bDead)
		{
			local_Enemy_ActionValue.Add(ArrElement, ArrElement->ActionValue);
			local_CharacterQueue.Add(ArrElement, ArrElement->ActionValue);
		}
		else
		{
			//TBD 被消灭的敌人解绑对应函数
			deadEnemyRefArr.Add(ArrElement);
		}
	}
	for (auto ArrElement : playerRefArr)
	{
		if (!ArrElement->bDead)
		{
			local_Player_ActionValue.Add(ArrElement, ArrElement->ActionValue);
			local_CharacterQueue.Add(ArrElement, ArrElement->ActionValue);
		}
		else
		{
			//TBD 被消灭的角色解绑对应函数
			deadPlayerRefArr.Add(ArrElement);
		}
	}
	//刷新数值，排除不可行动角色
	local_Enemy_ActionValue.GenerateKeyArray(enemiesRefArr);
	local_Player_ActionValue.GenerateKeyArray(playerRefArr);
	//TBD 根据行动值排序,新建映射的原因是，需要遍历的对象会在循环中被更改，所以保存备份，根据备份信息对原实例进行修改。
	TMap<ACharacter*, float>local_NumDummy;
	local_NumDummy = local_CharacterQueue;
	for (auto ArrElement : local_NumDummy)
	{
		//依次找到最小值，存入数组后删除
		TArray<float> localFloats;
		TArray<ACharacter*> localCharacters;
		int32 minIndex;
		float minValue;
		local_CharacterQueue.GenerateValueArray(localFloats);
		local_CharacterQueue.GenerateKeyArray(localCharacters);
		UKismetMathLibrary::MinOfFloatArray(localFloats, minIndex, minValue);
		local_SortedCharacters.Add(localCharacters[minIndex]);
		local_CharacterQueue.Remove(localCharacters[minIndex]);
	}
	//检查接口是否可用，若可用，则调用方法，获取行动值最小的角色对应的行动值（用接口）
	if (local_SortedCharacters[0] == nullptr) return;
	ICombatInterface* tempInterface1 = Cast<ICombatInterface>(local_SortedCharacters[0]);
	if (tempInterface1 == nullptr) return;
	tempInterface1->Int_GetActionValue(local_WinnerActionVal);

	// 更新其余的角色行动值
	for (auto ArrayElem : local_SortedCharacters)
	{
		ICombatInterface* tempInterface2 = Cast<ICombatInterface>(ArrayElem);
		if (tempInterface2 == nullptr) return;
		tempInterface2->Int_UpdateActionValue(local_WinnerActionVal);
	}

	//更新UI执行顺序 隐藏锁定图标
	BattleLayOut->RefreshActionOrder(local_SortedCharacters);

	for (auto ArrElement : enemiesRefArr)
	{
		ArrElement->UpdateLockIcon(true);
	}

	//检查战斗是否结束
}

void UTBG_BattleManager::LoadBattleUI()
{
	if (ExplorePlayerRef != nullptr)
	{
		ATBG_PlayerControllerBase* PC = ExplorePlayerRef->GetTBGPlayerController();
		PC->LoadBattleUI(this);
		BattleLayOut = PC->BattleLayOut;
	}
}
