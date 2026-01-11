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
#include "Interface/AnimInterface.h"
#include "Utilities/CF_SR.h"
#include "Utilities/GameInst_SR.h"

void ATBG_BattleManager::InitBattle(ATBG_Character_ExploreEnemies* InEnemyRef, ATBG_Character_ExplorePlayer* InPlayerRef)
{
	ExploreEnemyRef = InEnemyRef;
	ExplorePlayerRef = InPlayerRef;
	EnemyTeamInfo = InEnemyRef->EnemyTeamInfo;
	PlayerTeamInfo = InPlayerRef->PlayerTeamInfo;
	bBOSSFight = ExploreEnemyRef->bBOSSFight;
	//初始化战场
	PreInitializeBattle();
	//展示敌人阵容的时间
	GetWorld()->GetTimerManager().SetTimer(DisplayEnemyTimeHandle, this,&ATBG_BattleManager::PostInitialzeBattle,EnemyDisplayTime,false);
}

void ATBG_BattleManager::PreInitializeBattle()
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

void ATBG_BattleManager::PostInitialzeBattle()
{
	//改变阶段
	ProgressPhase = EProgressPhase::PP_B1_CalculateActionValue;
	// TBD - 若探索模式下玩家角色被敌人击中，则敌人先手攻击

	// TBD - 进入B1b阶段，计算行动值
	CalculateActionValue();
}

void ATBG_BattleManager::BattleEnd(EBattleFlags endResult)
{
	//胜利则退出战斗状态，回到探索，若失败，则退出游戏
}

void ATBG_BattleManager::HandlePlayerAttack(ATBG_Character_BattlePlayer* InPlayer)
{
	//GEngine->AddOnScreenDebugMessage(0, 10.f, FColor::Black, TEXT("player!"));
	ProgressPhase = EProgressPhase::PP_B2a_PlayerActionTime;
	ActivePlayer = InPlayer;
	//临时处理
	DisplayLockedIconsAndSetTargets();
	//更新UI 
	if (!ActivePlayer)return;
	BattleLayOut->SwitchATKMode(ActivePlayer->attackType);
	if (bBOSSFight)
	{
		//固定视角就什么都不做
	}
	else
	{
		//可选择隐藏其他角色
		SwitchAndHideOtherPlayerChars(true, ActivePlayer);
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->
			SetViewTargetWithBlend(ActivePlayer);
	}
}

void ATBG_BattleManager::HandleEnemyAttack(ATBG_Character_BattleEnemies* InEnemy)
{
	//GEngine->AddOnScreenDebugMessage(0, 10.f, FColor::Black, TEXT("Enemy!"));
	ProgressPhase = EProgressPhase::PP_B2b_EnemyActionTime;
	ActiveEnemy = InEnemy;
}

void ATBG_BattleManager::TurnEnd(AActor* endTurnActor, bool bConsumeTurn)
{
	//回合结束的逻辑
}

void ATBG_BattleManager::ChangeCameraAndStopMovement()
{
	// 生成Battle Pawn，控制它并使用在其中定义的按键
	BattlePawn = Cast<ATBG_BattlePawn>(
		GetWorld()->SpawnActor<AActor>(BattlePawnClass, FVector(0, 0, 0), FRotator(0, 0, 0)));

	GetWorld()->GetFirstPlayerController()->Possess(BattlePawn);

	// 探索玩家角色停止奔跑，不可移动
	ExploreEnemyRef->GetCharacterMovement()->SetMovementMode(MOVE_None);

	// 改变摄像机视角
	ACameraActor* targetCA;

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
ACameraActor* ATBG_BattleManager::RetrieveCamera(FName tag)
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

void ATBG_BattleManager::InitSpawnPostion()
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

void ATBG_BattleManager::RetrieveEnemyPosition(int32 PosIndex, FVector& TargetPos, float& yaw)
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
void ATBG_BattleManager::RetrievePlayerPosition(int32 PosIndex, FVector& TargetPos, float& yaw)
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
void ATBG_BattleManager::SpawnEnemiesAndDecideLocation()
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
void ATBG_BattleManager::SpawnPlayerAndDecideLocation()
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

void ATBG_BattleManager::CalculateActionValue()
{

	//定义排序相关本地变量
	TMap< ATBG_Character_BattleEnemies*, float> local_Enemy_ActionValue;
	TMap< ATBG_Character_BattlePlayer*, float> local_Player_ActionValue;
	TMap<ACharacter*, float>local_CharacterQueue;
	TArray<ACharacter*> local_SortedCharacters;
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
		//对应变量为bHiddenInGame，为假时显示
		ArrElement->UpdateLockIcon(true);
	}

	//检查战斗是否结束
	EBattleFlags CurBattleFlag = EBattleFlags::BF_EMAX;
	CurBattleFlag = CheckGameOver(local_Enemy_ActionValue, local_Player_ActionValue);
	switch (CurBattleFlag)
	{
		case EBattleFlags::BF_EMAX:
			return;
			break;
		case EBattleFlags::BF_ContinueBattle:
			//跳出，继续执行后续内容
			break;
		case EBattleFlags::BF_PlayerWin:
			BattleEnd(CurBattleFlag);
			break;
		case EBattleFlags::BF_EnemyWin:
			BattleEnd(CurBattleFlag);
			break;
		default:
			return;
			break;
	}
	// 当前行动对象进入准备状态（判断敌人和玩家的数组中行动值谁最小）
	TArray<float> l_LocalFloatsEAV;
	TArray<float> l_LocalFloatsPAV;
	int32 minIndex_E;
	float minValue_E;
	int32 minIndex_P;
	float minValue_P;
	local_Enemy_ActionValue.GenerateValueArray(l_LocalFloatsEAV);
	local_Player_ActionValue.GenerateValueArray(l_LocalFloatsPAV);
	UKismetMathLibrary::MinOfFloatArray(l_LocalFloatsEAV, minIndex_E, minValue_E);
	UKismetMathLibrary::MinOfFloatArray(l_LocalFloatsPAV, minIndex_P, minValue_P);
	if (minValue_E > minValue_P)
	{
		// 玩家角色的行动值小，玩家行动
		TArray<ATBG_Character_BattlePlayer*> l_LocalPlayerChars;
		local_Player_ActionValue.GenerateKeyArray(l_LocalPlayerChars);
		HandlePlayerAttack(l_LocalPlayerChars[minIndex_P]);
	}
	else
	{
		// 敌人的行动值小，敌人行动
		TArray< ATBG_Character_BattleEnemies*> l_LocalEnemyChars;
		local_Enemy_ActionValue.GenerateKeyArray(l_LocalEnemyChars);
		HandleEnemyAttack(l_LocalEnemyChars[minIndex_E]);
	}
}

EBattleFlags ATBG_BattleManager::CheckGameOver(TMap<ATBG_Character_BattleEnemies*, float> eArr, TMap<ATBG_Character_BattlePlayer*, float> pArr)
{
	if (eArr.Num() == 0)return EBattleFlags::BF_PlayerWin;
	if (pArr.Num() == 0)return EBattleFlags::BF_PlayerWin;
	return EBattleFlags::BF_ContinueBattle;
}

void ATBG_BattleManager::SwitchEnemyLockIcon(bool bNext)
{
	//我方行动回合外，直接跳出
	if (ProgressPhase != EProgressPhase::PP_B2a_PlayerActionTime)return;
	//复活技能处理
	if (NotResurrectSkill())
	{
		//根据按键查找对应敌人的Key值
		CalculateLockIndex(bNext);
	}
	else
	{
		if (indexForLockedTarget == -1)
		{
			//没有有效目标
		}
		else
		{
			CalculateLockIndex(bNext);
		}
	}
}

void ATBG_BattleManager::SetMultipleEnemyLocks()
{
	//是否范围攻击
	if (IsMutipleTargets())
	{
		//GEngine->AddOnScreenDebugMessage(0, 10.f, FColor::Black, TEXT("attack all!"));
		//范围攻击
		currentEnemyTargets.Empty();
		//注意顺序
		if(enemiesRefArr.IsValidIndex(indexForLockedTarget))
		{
			currentEnemyTargets.Add(enemiesRefArr[indexForLockedTarget]);
		}
		if (enemiesRefArr.IsValidIndex(indexForLockedTarget-1))
		{
			currentEnemyTargets.Add(enemiesRefArr[indexForLockedTarget-1]);
		}		
		if (enemiesRefArr.IsValidIndex(indexForLockedTarget+1))
		{
			currentEnemyTargets.Add(enemiesRefArr[indexForLockedTarget+1]);
		}
		ShowEnemyLockIconMultiple(currentEnemyTargets);
	}
	else//单体显示一个锁定
	{
		currentEnemyTarget = enemiesRefArr[indexForLockedTarget];
		lastClickedActor = currentEnemyTarget;
		ShowEnemyLockIconByIndex(indexForLockedTarget);
	}
}

bool ATBG_BattleManager::IsMutipleTargets()
{
	if (!ActivePlayer) return false;
	bool result;
	if (ActivePlayer->playerAtr.MultipleTargets.Contains(ActivePlayer->attackType))
	{
		result = *(ActivePlayer->playerAtr.MultipleTargets.Find(ActivePlayer->attackType));
	}
	else
	{
		return false;
	}
	return result;
}

bool ATBG_BattleManager::NotResurrectSkill()
{
	if (!ActivePlayer) return false;
	bool result = false;
	if (ActivePlayer->playerAtr.BuffSkillStats.Contains(ActivePlayer->attackType))
	{
		FBuffInfo tempBuffType = *(ActivePlayer->playerAtr.BuffSkillStats.Find(ActivePlayer->attackType));
		result = (tempBuffType.BuffType != EBuffTypes::BT_Resurrection);
	}
	else
	{
		//未找到，则不是复活技能
		result = true;
	}

	return result;
}

bool ATBG_BattleManager::IsBuffTarget()
{
	if (!ActivePlayer) return false;
	bool result = ActivePlayer->playerAtr.BuffSkillStats.Contains(ActivePlayer->attackType);
	return result;
}

bool ATBG_BattleManager::IsMeleeAction()
{
	if (!ActivePlayer) return true;
	bool result;
	if(ActivePlayer->playerAtr.MeleeAction.Contains(ActivePlayer->attackType))
	{
		result = *(ActivePlayer->playerAtr.MeleeAction.Find(ActivePlayer->attackType));
	}
	else
	{
		return true;
	}
	return result;
}

void ATBG_BattleManager::ShowEnemyLockIconByIndex(int32 Index)
{
	for (auto ArrElement : enemiesRefArr)
	{
		ArrElement->UpdateLockIcon(true);
	}
	//若有效，则开启锁定
	if (enemiesRefArr[Index])
	{
		enemiesRefArr[Index]->UpdateLockIcon(false);
	}
}

void ATBG_BattleManager::ShowEnemyLockIconMultiple(TArray<ATBG_Character_BattleEnemies*> InCurrentEnemyTargets)
{
	for (auto ArrElement : enemiesRefArr)
	{
		ArrElement->UpdateLockIcon(true);
	}
	for (auto ArrElement : InCurrentEnemyTargets)
	{
		if (!ArrElement)return;
		if (ArrElement->bDead)return;
		ArrElement->UpdateLockIcon(false);
	}
}
void ATBG_BattleManager::ShowPlayerLockIconByIndex(int32 Index)
{
	for (auto ArrElement : playerRefArr)
	{
		ArrElement->UpdateLockIcon(true);
	}
	//若有效，则开启锁定
	if (playerRefArr[Index])
	{
		playerRefArr[Index]->UpdateLockIcon(false);
	}
}
void ATBG_BattleManager::ShowPlayerLockIconMultiple(TArray<ATBG_Character_BattlePlayer*> InCurrentPlayerTargets)
{
	for (auto ArrElement : playerRefArr)
	{
		ArrElement->UpdateLockIcon(true);
	}
	for (auto ArrElement : InCurrentPlayerTargets)
	{
		if (ArrElement)
		{
			ArrElement->UpdateLockIcon(false);
		}
	}
}

void ATBG_BattleManager::CalculateLockIndex(bool bNext)
{
	int32 t1 = -1;
	if (IsBuffTarget())
	{	//是否复活魔法
		if (NotResurrectSkill())
		{
			//不是复活魔法，就只选还活着的角色
			t1 = playerRefArr.Num(); 
		}
		else
		{
			t1 = deadPlayerRefArr.Num();
		}
	}
	else
	{
		t1 = enemiesRefArr.Num();
	}
	//确定对比的对象数量。
	if (!bNext)
	{
		// TBD - 是否释放增益魔法？如回血、复活、拉条等
		if ((indexForLockedTarget + 1) <t1)
		{
			indexForLockedTarget++;
		}
		else
		{
			indexForLockedTarget = 0;
		}
	}
	else
	{
		if (indexForLockedTarget > 0)
		{
			indexForLockedTarget--;
		}
		else
		{
			// TBD - 是否释放增益魔法？如回血、复活、拉条等
			indexForLockedTarget = t1 - 1;
		}
	}
	//是否释放复活魔法
	if (NotResurrectSkill())
	{
		if (IsBuffTarget())
		{
			//buff技能锁友方
			SetPlayerLockedIcons();
		}
		else
		{	//不是复活技能，不是buff技能，就锁定敌人
			SetMultipleEnemyLocks();
		}
	}
	else
	{
		SetDeadPlayerLockedIcons();
	}

	 
	//TBD是否释放增益模式
	//设置敌人锁定目标
	SetMultipleEnemyLocks();
}

void ATBG_BattleManager::SetDeadPlayerLockedIcons()
{
	if (!deadPlayerRefArr.IsValidIndex(indexForLockedTarget)) return;
	//隐藏所有的锁定的图标
	HideAllLockedIcons();
	//锁定
	currentPlayerTarget = deadPlayerRefArr[indexForLockedTarget];
	lastClickedActor = currentPlayerTarget;

	if (currentPlayerTarget)
	{
		currentPlayerTarget->UpdateLockIcon(false);
	}
}

void ATBG_BattleManager::SetPlayerLockedIcons()
{
	if (IsMutipleTargets())
	{
		currentPlayerTargets = playerRefArr;
		//向友方群体释放技能，显示锁定图标
		ShowPlayerLockIconMultiple(currentPlayerTargets);
	}
	else
	{
		if (!playerRefArr.IsValidIndex(indexForLockedTarget))return;
		currentPlayerTarget = playerRefArr[indexForLockedTarget];
		lastClickedActor = currentPlayerTarget;
		ShowPlayerLockIconByIndex(indexForLockedTarget);
	}
}

void ATBG_BattleManager::HideAllLockedIcons()
{
	for (auto ArrElement : playerRefArr)
	{
		ArrElement->UpdateLockIcon(true);
	} 
	for (auto ArrElement : enemiesRefArr)
	 {
		ArrElement->UpdateLockIcon(true);
	}
	for (auto ArrElement : deadEnemyRefArr)
	{
		ArrElement->UpdateLockIcon(true);
	}
	for (auto ArrElement : deadPlayerRefArr)
	{
		ArrElement->UpdateLockIcon(true);
	}
}

void ATBG_BattleManager:: DisplayLockedIconsAndSetTargets()
{
	// 隐藏所有锁定图标
	HideAllLockedIcons();
	// TBD - 播放面板动画
	BattleLayOut->HandleStatsPanelAnimating(ActivePlayer,true);
	// 是否切换到释放增益魔法时，选择玩家角色的视角
	if (IsBuffTarget())
	{
		UpdatePlayerLockedIconToMultiple();
	}
	else
	{
		UpdateEnemyLockedIconToMultiple();
	}
}
void ATBG_BattleManager::UpdatePlayerLockedIconToMultiple()
{
	if (NotResurrectSkill())
	{
		//锁定的目标锁到玩家中间
		indexForLockedTarget = (playerRefArr.Num() - 1) / 2;
		SetPlayerLockedIcons();
	}
	else
	{
		if (deadPlayerRefArr.Num() != 0)
		{
			indexForLockedTarget = (deadPlayerRefArr.Num() - 1) / 2;
		}
		else
		{
			//无目标
			indexForLockedTarget = -1;
		}
		if (!deadPlayerRefArr.IsValidIndex(indexForLockedTarget))return;
		if (deadPlayerRefArr[indexForLockedTarget])
		{
			deadPlayerRefArr[indexForLockedTarget]->UpdateLockIcon(false);
		}
	}
}
void ATBG_BattleManager::UpdateEnemyLockedIconToMultiple()
{
	TArray<ATBG_Character_BattleEnemies*> local_ValidEnemies;
	//筛选敌人角色，防止大招插队造成导致的enemyarr未更新导致的无效角色选择
	for (auto ArrElement : enemiesRefArr)
	{
		if (!ArrElement)return;

		if (ArrElement->bDead)
		{
			local_ValidEnemies.Add(ArrElement);
		}
	}
	indexForLockedTarget = (local_ValidEnemies.Num() - 1) / 2;
	if (!enemiesRefArr.IsValidIndex(indexForLockedTarget))return;
	SetMultipleEnemyLocks();
}
void ATBG_BattleManager::SwitchAndHideOtherPlayerChars(bool bHideOther, ATBG_Character_BattlePlayer* activePlayer)
{
	//TBD_隐藏人物
	TArray<ATBG_Character_BattlePlayer*> l_tempPlayers;
	TeamInstForUI.GenerateValueArray(l_tempPlayers);
	for (auto ArrElement : l_tempPlayers)
	{
		ArrElement->SetHiddenForPlayer(bHideOther);
	}
	activePlayer->SetHiddenForPlayer(false);
}
void ATBG_BattleManager::ExecuteAction(EAttackType ATKType)
{
	//根据传入变量执行不同的动作
	if (!ActivePlayer)return;
	switch (ATKType)
	{
		{
	case EAttackType::AT_EMAX:
		break;
	case EAttackType::AT_NormalATK:
		if (ProgressPhase == EProgressPhase::PP_B2a_PlayerActionTime &&
			ActivePlayer->attackType != EAttackType::AT_Ultimate)
		{
			HandlePlayerATK(EAttackType::AT_NormalATK);
		}
		break;
	case EAttackType::AT_SkillATK:
		if (ProgressPhase == EProgressPhase::PP_B2a_PlayerActionTime &&
			ActivePlayer->attackType != EAttackType::AT_Ultimate)
		{
			HandlePlayerATK(EAttackType::AT_SkillATK);
		}
		break;
	case EAttackType::AT_FollowTK:
		break;
	case EAttackType::AT_Ultimate:
		ExecuteUltimate();
		break;
	case EAttackType::AT_DelayATK_E:
		break;
	default:
		break;
		}
	}
}


void ATBG_BattleManager::HandlePlayerATK(EAttackType ATKType)
{
	if (!ActivePlayer)return;
	if (ATKType == EAttackType::AT_SkillATK)
	{
		if (skillPoints <= 0)
		{
			//播放技能失败声效
			UGameplayStatics::SpawnSound2D(GetWorld(), UnableSFX);
		}
	}
	//状态和输入不同则改变攻击方式并更新目标显示 
	if (ActivePlayer->attackType != ATKType)
	{
		ActivePlayer->attackType = ATKType;
		DisplayLockedIconsAndSetTargets();
		//判定是否切换镜头
		CameraForBuffSelections();
		//更改UI高亮状态 
		BattleLayOut->SwitchATKMode(ATKType);
	}
	else
	{
		if (ATKType == EAttackType::AT_Ultimate)
		{
			if (IAnimInterface* tempInterface = Cast<IAnimInterface>(ActivePlayer->GetMesh()->GetAnimInstance()))
			{
				tempInterface->Int_SetUltimateReadeVFX(false);
			}
		}
		// 根据技能配置的目标阵营和涉及数量，修改目标的数量、敌我方
		if (IsMutipleTargets())
		{

			bool l_bCousumTurn = (ATKType == EAttackType::AT_NormalATK || ATKType == EAttackType::AT_SkillATK);

			if (IsBuffTarget())
			{
				if (!currentPlayerTargets.IsValidIndex(0))return;
				if (!currentPlayerTargets[0])return;
		
				HideAllLockedIcons();
				ProgressPhase = EProgressPhase::PP_B2c_Animating;

				//父类数组无法存放子类数组，需要将子类数组传入用于构建。
				TArray<AActor*> l_tempTargets(currentPlayerTargets);
				ActivePlayer->MultipleATK(l_tempTargets, l_bCousumTurn, IsMeleeAction(), ATKType);
			}
			else
			{
				if (!currentEnemyTargets.IsValidIndex(0))return;
				if (!currentEnemyTargets[0])return;

				HideAllLockedIcons();
				ProgressPhase = EProgressPhase::PP_B2c_Animating;

				//父类数组无法存放子类数组，需要将子类数组传入用于构建。
				TArray<AActor*> l_tempTargets(currentEnemyTargets);
				ActivePlayer->MultipleATK(l_tempTargets, l_bCousumTurn, IsMeleeAction(), ATKType);
			}
	
		}
		else
		{

			AActor* tempTargetActor;
			bool l_bCousumTurn = (ATKType == EAttackType::AT_NormalATK || ATKType == EAttackType::AT_SkillATK);
			if(IsBuffTarget())
			{
				if (!currentPlayerTarget)return;
				tempTargetActor = currentPlayerTarget;
			}
			else 
			{
				if (!currentEnemyTarget)return;
				tempTargetActor = currentEnemyTarget;
			}
			HideAllLockedIcons();
			ProgressPhase = EProgressPhase::PP_B2c_Animating;

			ActivePlayer->SingleATK(tempTargetActor, l_bCousumTurn, IsMeleeAction(),ATKType);
		}

		//根据攻击类型的不同，增加/减少技能点。
		int32 l_skillPoints = skillPoints + 0;
		int32 l_skillPointDelta = 0;
		switch (ATKType)
		{
			case EAttackType::AT_EMAX:
				break;
			case EAttackType::AT_NormalATK:
				l_skillPointDelta = 1;
				break;
			case EAttackType::AT_SkillATK:
				l_skillPointDelta = -1;
				break;
			case EAttackType::AT_FollowTK:
				break;
			case EAttackType::AT_Ultimate:
				break;
			case EAttackType::AT_DelayATK_E:
				break;
			default:
				break;
		}
		skillPoints = UKismetMathLibrary::Clamp(l_skillPoints + l_skillPointDelta, 0, 5);
	}
	//UI文本提示
	BattleLayOut->HandlePhaseHintFromCpp(ActivePlayer, ATKType);
	//战斗状态下隐藏其他界面
	BattleLayOut->HideATKButtons();
}

void ATBG_BattleManager::ExecuteUltimate()
{
	//检查大招序列是否>0,且玩家回合
	if (ProgressPhase == EProgressPhase::PP_B2a_PlayerActionTime && ultimatePlayerQueue.Num()>0)
	{
		HandlePlayerATK(EAttackType::AT_Ultimate);
	}
	//敌方回合，但玩家按下大招
}
void ATBG_BattleManager::CameraForBuffSelections()
{
	if (IsBuffTarget())
	{
		//释放增益魔法的逻辑,默认锁定自身
		SwitchAndHideOtherPlayerChars(false, ActivePlayer);
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->
			SetViewTargetWithBlend(RetrieveCamera(FName(*buffCA)));
	}
	else
	{
		if (bBOSSFight)
		{
			UGameplayStatics::GetPlayerController(GetWorld(), 0)->
				SetViewTargetWithBlend(RetrieveCamera(FName(*fixedCA)));
		}
		else
		{
			//可选择隐藏其他角色
			SwitchAndHideOtherPlayerChars(true, ActivePlayer);
			UGameplayStatics::GetPlayerController(GetWorld(), 0)->
				SetViewTargetWithBlend(ActivePlayer);
		}
	}
}

void ATBG_BattleManager::BeginPlay()
{
	Super::BeginPlay();
}

void ATBG_BattleManager::LoadBattleUI() 
{
	if (BattleLayoutClassRef)
	{
		BattleLayOut = CreateWidget<UBattleLayOut>(GetWorld(), BattleLayoutClassRef);
		if (BattleLayOut)
		{
			BattleLayOut->ConstructDeferred(this);
			BattleLayOut->AddToViewport();
		}
	}
}
