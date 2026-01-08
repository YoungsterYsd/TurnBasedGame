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

void UTBG_BattleManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	////生成battlePawn，使用其中自定义的按键
	//TArray<AActor*> tempActors;
	//// 获取场景中所有CameraActors
	//UGameplayStatics::GetAllActorsOfClass(GEngine->GameViewport->GetWorld(), ACameraActor::StaticClass(), tempActors);
	//for (auto ArrayElem : tempActors)
	//{
	//	camerasArr.Add(Cast<ACameraActor>(ArrayElem));
	//}

	//// 获取敌人的位置
	//tempActors.Empty();
	//UGameplayStatics::GetAllActorsOfClass(GEngine->GameViewport->GetWorld(), ATargetPoint::StaticClass(), tempActors);
	//for (auto ArrayElem : tempActors)
	//{
	//	if (ArrayElem->ActorHasTag("EnemyPos"))
	//	{
	//		enemySpawnPointsArr.Add(ArrayElem);
	//	}
	//}

	//// 获取玩家角色的位置
	//tempActors.Empty();
	//UGameplayStatics::GetAllActorsOfClass(GEngine->GameViewport->GetWorld(), ATargetPoint::StaticClass(), tempActors);
	//for (auto ArrayElem : tempActors)
	//{
	//	if (ArrayElem->ActorHasTag("PlayerPos"))
	//	{
	//		playerSpawnPointsArr.Add(ArrayElem);
	//	}
	//}
}

void UTBG_BattleManager::Deinitialize()
{
	Super::Deinitialize();
}

void UTBG_BattleManager::InitBattle(ATBG_Character_ExploreEnemies* InEnemyRef, ATBG_Character_ExplorePlayer* InPlayerRef)
{
	ExploreEnemyRef = InEnemyRef;
	ExplorePlayerRef = InPlayerRef;
	EnemyTeamInfo = ExploreEnemyRef->EnemyInfo;
	//初始化战场
	PreInitializeBattle();
}

void UTBG_BattleManager::PreInitializeBattle()
{
	//改变战斗阶段
	ProgressPhase = EProgressPhase::PP_A1_PreInitialization;
	//清理旧缓存
	ememiesRefArr.Empty();
	deadEnemyRefArr.Empty();
	playerRefArr.Empty();
	deadPlayerRefArr.Empty();
	//相机控制
	ChangeCameraAndStopMovement();
	// TBD - 设置展示敌人阵容的时间，测试阶段设为0.2s

	// TBD - 若探索模式下玩家角色被敌人击中，则敌人先手攻击

	// TBD - 进入B1b阶段，计算行动值
}

void UTBG_BattleManager::ChangeCameraAndStopMovement()
{
	// 生成Battle Pawn，控制它并使用在其中定义的按键
	BattlePawn = Cast<ATBG_BattlePawn>(
		GetWorld()->SpawnActor<AActor>(ExplorePlayerRef->GetTBGPlayerController()->BattlePawnClass, FVector(0, 0, 0), FRotator(0, 0, 0)));

	GetWorld()->GetFirstPlayerController()->Possess(BattlePawn);

	// 探索玩家角色停止奔跑，不可移动
	ExploreEnemyRef->GetCharacterMovement()->SetMovementMode(MOVE_None);

	// 改变摄像机视角
	//ACameraActor* targetCA;
	//FString normalCA = "tag_c_start_normal";
	//FString fixedCA = "tag_c_boss";
	//if (bBOSSFight)
	//{
	//	targetCA = RetrieveCamera(FName(*fixedCA));
	//}
	//else
	//{
	//	targetCA = RetrieveCamera(FName(*normalCA));
	//} 

	//if (targetCA == nullptr) return;
	//ATBG_PlayerControllerBase* PC =
	//Cast<ATBG_PlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	//if (PC == nullptr) return;
	//PC->SetViewTargetWithBlend(targetCA);

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
