// YangSida Allright reserved


#include "Character/TBG_Character_BattlePlayer.h"
#include "Character\TBG_Character_BattleEnemies.h"
#include "Kismet\KismetMathLibrary.h"
#include "Utilities/CF_SR.h"
#include "TBG_BattleManager.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Actor/FloatingIndicator.h"

ATBG_Character_BattlePlayer::ATBG_Character_BattlePlayer()
{
	MarkedIcon = CreateDefaultSubobject<UWidgetComponent>("Marked Icon");
	MarkedIcon->SetupAttachment(RootComponent);
	MarkedIcon->bHiddenInGame = true;
	PrimaryActorTick.bCanEverTick = true;
}

void ATBG_Character_BattlePlayer::UpdateLockIcon(bool bHide)
{
	MarkedIcon->bHiddenInGame = bHide;
}

void ATBG_Character_BattlePlayer::Int_RefreshActionValueBySpeed()
{
	ActionValue = Distance/ playerAtr.Speed;
}

void ATBG_Character_BattlePlayer::InitializeCharData()
{
	//初始化UI相关
	bPlayerFaction = true;
	AvatarIcon = playerAtr.CharIcon_Banner;
	maxHP = playerAtr.HP;
	curHP = maxHP;
	maxEnergy = playerAtr.Energy;
	curEnergy = 0;
	maxToughness = 0;
	curToughness = maxToughness;
	GetMesh()->SetSkeletalMesh(playerAtr.SKM);
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(playerAtr.AnimClass);
	OringinalLocation = GetActorLocation();
	OriginalRotation = GetActorRotation();

	// 初始化行动值
	Int_RefreshActionValueBySpeed();
	//TBD - 初始化镜头角度
}

void ATBG_Character_BattlePlayer::PlayAnimationAndTimeline()
{
	//播放转向目标的时间轴
	RotateToTarget_TL.PlayFromStart();
	//显示当前玩家角色。
	SetHiddenForPlayer(false);
	//是否近战
	if (Melee)
	{
		//近战逻辑
		//是否固定视角？
		if (!UCF_SR::Flib_GetBM()->bBOSSFight)
		{
			//普通战切换视角
			float l_blendTime = 0.6f;
			if (attackType == EAttackType::AT_FollowTK)
			{
				l_blendTime = 0.f;
			}
			else
			{
				l_blendTime = 0.3f;
			}
			UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(rotateToTarget, l_blendTime);
		}
		else
		{
			//boss战不切换视角
		}
		//播放指定的动画
		PlaySpecifiedAnim("Slide_F");
		//TBD 时间轴移动
		Slide_F_TL.PlayFromStart();
		//Slide_B_TL.PlayFromStart();
	}
	else
	{
		//远程逻辑
		PlayATKAnimByATKType();
	}
}

void ATBG_Character_BattlePlayer::SetHiddenForPlayer(bool bCustomHidden)
{
	SetActorHiddenInGame(bCustomHidden);
	//TBD如果套盾了 也要隐藏
}

float ATBG_Character_BattlePlayer::PlaySpecifiedAnim(FString str)
{
	float l_animTime = 0.f;
	if (playerAtr.Montages.Contains(str))
	{
		l_animTime = PlayAnimMontage(*playerAtr.Montages.Find(str));
	}
	return l_animTime;
}

void ATBG_Character_BattlePlayer::PlayATKAnimByATKType()
{
	FString specifiedActionString;
	switch(attackType)
	{
	case EAttackType::AT_EMAX:
		break;
	case EAttackType::AT_NormalATK:
		specifiedActionString = "normalATK";
		break;
	case EAttackType::AT_SkillATK:
		specifiedActionString = "skillATK";
		break;
	case EAttackType::AT_FollowTK:
		specifiedActionString = "followATK";
		break;
	case EAttackType::AT_Ultimate:
		specifiedActionString = "ultimateATK";
		break;
	case EAttackType::AT_DelayATK_E:
		break;
	default:
		break;
	}
	float animTime = PlaySpecifiedAnim(specifiedActionString);
	//播放动画后执行下一段逻辑
	//能量修改
	HandleEP(attackType, false, 0.0f);

	//计时器
	if (Melee)
	{
		GetWorldTimerManager().SetTimer(AfterPlayeringMeleeATKAnimHandle, this,
			&ATBG_Character_BattlePlayer::AfterPlayingMeleeATKAnim, animTime, false);
	}
	else
	{
		GetWorldTimerManager().SetTimer(AfterPlayeringMeleeATKAnimHandle, this,
			&ATBG_Character_BattlePlayer::GeneralPlayerAttackOver, animTime, false);
	}

}

void ATBG_Character_BattlePlayer::AfterPlayingMeleeATKAnim()
{
	//播放跳回的蒙太奇
	PlaySpecifiedAnim("Slide_B");
	//执行回城时间轴
	Slide_B_TL.PlayFromStart();
	//
}

void ATBG_Character_BattlePlayer::GeneralPlayerAttackOver()
{
	//恢复转向，进入下一阶段
	RotateToTarget_TL.ReverseFromEnd();
	UCF_SR::Flib_GetBM()->TurnEnd(this, ConsumeTurn);
}

void ATBG_Character_BattlePlayer::CalculateDmg(bool bBuff, float& hpDmg, float& toughnessDmg)
{
	float f_hpDmg;
	float f_toughnessDmg;
	float f_ratio = 0.0f;

	// 根据不同动作确定伤害系数 f_ratio
	switch (attackType)
	{
	case EAttackType::AT_EMAX:
		f_ratio = 0.0f;
		break;
	case EAttackType::AT_NormalATK:
		f_ratio = 1.0f;
		break;
	case EAttackType::AT_SkillATK:
		f_ratio = 1.0f;
		break;
	case EAttackType::AT_FollowTK:
		f_ratio = 1.0f;
		break;
	case EAttackType::AT_Ultimate:
		f_ratio = playerAtr.UltimateRatio;
		break;
	case EAttackType::AT_DelayATK_E:
		f_ratio = 1.0f;
		break;
	default:
		f_ratio = 0.0f;
		break;
	}

	if (bBuff)
	{
		f_hpDmg = playerAtr.Attack / float(attackCountIOC) * f_ratio * buffInfo.BuffRatio;
	}
	else
	{
		f_hpDmg = playerAtr.Attack / float(attackCountIOC) * f_ratio;
	}

	f_toughnessDmg = playerAtr.Attack / float(attackCountIOC) * f_ratio; // 可以再乘以破盾·削韧效率系数

	// 将值传出
	hpDmg = f_hpDmg;
	toughnessDmg = f_toughnessDmg;
}

void ATBG_Character_BattlePlayer::SetDelayedMark(bool bNewVisibility)
{
	MarkedIcon->bHiddenInGame=!bNewVisibility;
}

void ATBG_Character_BattlePlayer::HandleEP(EAttackType ATKType, bool bDirect, float val)
{
	//更新能量ep
	// 更新能量值EP
	// 参考：释放战技回复30点，普攻回复20点，大招返还5点，受击单体攻击回复10点，受到AOE攻击回复10点，
	// 击杀敌人获得10点，追加攻击回复5-10点；均受到“能量回复效率（充能效率）”值影响

	float l_deltaEP = 0.0f;

	switch (ATKType)
	{
	case EAttackType::AT_EMAX:
		break;
	case EAttackType::AT_NormalATK:
		l_deltaEP = 20.0f;
		break;
	case EAttackType::AT_SkillATK:
		l_deltaEP = 30.0f;
		break;
	case EAttackType::AT_FollowTK:
		l_deltaEP = 8.0f;
		break;
	case EAttackType::AT_Ultimate:
		l_deltaEP = 0.0f;
		break;
	case EAttackType::AT_DelayATK_E:
		break;
	default:
		break;
	}

	if (bDirect) 
	{
		curEnergy = curEnergy + val;
	}
	else
	{
		curEnergy = curEnergy + l_deltaEP * 1.0f;
	}
	// 若扩展能量回复效率（充能效率），可把系数1.0f设为变量，额外控制

}

void ATBG_Character_BattlePlayer::GetAttributes(float& mHP, float& cHP, float& mEP, float& cEP, float& mT, float& cT)
{
	mHP = maxHP;
	cHP = curHP;
	mEP = maxEnergy;
	cEP = curEnergy;
	mT = maxToughness;
	cT = curToughness;
}

void ATBG_Character_BattlePlayer::TL_RotateToTarget(float deltaTime)
{
	//deltaTime 0-1 时间根据Curve而定

	//玩家转向锁定的Irene方向
	if (!rotateToTarget)return;
	if (!Cast<ATBG_Character_BattleEnemies>(rotateToTarget))return;
	{
		FRotator l_TargetRot = (rotateToTarget->GetActorLocation() - GetActorLocation()).Rotation();
		//最短路径
		FRotator l_tempRot = UKismetMathLibrary::RLerp(OriginalRotation, l_TargetRot, deltaTime,true);
		SetActorRotation(FRotator(0, l_tempRot.Yaw, 0));
	}

}

void ATBG_Character_BattlePlayer::TL_Slide_F(float deltaTime)
{
	//移动玩家角色
	FVector l_targetLoc = UKismetMathLibrary::VLerp(OringinalLocation, targetLocation, deltaTime);
	SetActorLocation(l_targetLoc);
}

void ATBG_Character_BattlePlayer::TL_SlideF_Finished()
{
	//停止蒙太奇
	GetMesh()->GetAnimInstance()->StopAllMontages(0.2f);
	//延迟0.2f，设置定时器
	GetWorldTimerManager().SetTimer(PlayATKAnimHandler, this,
		&ATBG_Character_BattlePlayer::PlayATKAnimByATKType, 0.2f, false);

}

void ATBG_Character_BattlePlayer::TL_Slide_B(float deltaTime)
{
	//移动玩家角色
	FVector l_targetLoc = UKismetMathLibrary::VLerp(targetLocation, OringinalLocation, deltaTime);
	SetActorLocation(l_targetLoc);
}

void ATBG_Character_BattlePlayer::TL_SlideB_Finished()
{
	// 延迟0.4s后，调用近战、远程通用战斗结束函数
	GetWorldTimerManager().SetTimer(MeleePlayerEndHandle, this,
		&ATBG_Character_BattlePlayer::GeneralPlayerAttackOver, 0.2f, false);
}

void ATBG_Character_BattlePlayer::SingleATK(AActor* target, bool bCounsumeTurn, bool bMelee, EAttackType ATKType)
{
	 //单体攻击逻辑
	attackType = ATKType;
	ConsumeTurn = bCounsumeTurn;
	Melee = bMelee;
	targetActor = target;
	rotateToTarget = target;
	//默认停留再敌人前方150cm处
	targetLocation = targetActor->GetActorLocation() + targetActor->GetActorForwardVector() * 150.f;
	PlayAnimationAndTimeline();

}

void ATBG_Character_BattlePlayer::MultipleATK(TArray<AActor*> targets, bool bCounsumeTurn, bool bMelee, EAttackType ATKType)
{
	//范围攻击逻辑
	attackType = ATKType;
	ConsumeTurn = bCounsumeTurn;
	Melee = bMelee;
	currentTargets = targets;
	if (!currentTargets.IsValidIndex(0))return;
	if (!currentTargets[0])return;
	targetActor = currentTargets[0];
	rotateToTarget = currentTargets[0];
	//默认停留再敌人前方150cm处
	targetLocation = targetActor->GetActorLocation() + targetActor->GetActorForwardVector() * 150.f;
	PlayAnimationAndTimeline();

}

void ATBG_Character_BattlePlayer::HandleShieldAndHP(float dmg)
{
	// 有套盾先扣盾值，盾值不够再扣血，无套盾直接扣血
	if (curToughness > 0.0f)
	{
		curToughness = curToughness - dmg;

		// 生成白色数字
		// 需传入显示的数字，故使用延迟生成
		FTransform CustomTransform3;
		CustomTransform3.GetLocation() = GetActorLocation();
		CustomTransform3.GetRotation() = FQuat(0, 0, 0, 0);
		CustomTransform3.GetScale3D() = FVector(1, 1, 1);
		AFloatingIndicator* l_FI3 = GetWorld()->SpawnActorDeferred<AFloatingIndicator>(
			FloatingIndicatorClass, CustomTransform3);
		l_FI3->floatingNum = dmg;
		l_FI3->specifiedColor = FColor::White;
		l_FI3->txtLocation = GetActorLocation();
		l_FI3->FinishSpawning(CustomTransform3);

		// 扣除后，是否还有剩余盾值
		if (curToughness > 0.0f)
		{
			// 三月七机制，发动攻击
		}
		else
		{
			// 如果没有，则把curThougness不足的部分加算到血量上
			// curToughness <= 0;
			curHP = curHP + curToughness;

			// 生成红色数字
			FTransform CustomTransform1;
			CustomTransform1.GetLocation() = GetActorLocation();
			CustomTransform1.GetRotation() = FQuat(0, 0, 0, 0);
			CustomTransform1.GetScale3D() = FVector(1, 1, 1);
			AFloatingIndicator* l_FI1 = GetWorld()->SpawnActorDeferred<AFloatingIndicator>(
				FloatingIndicatorClass, CustomTransform1);
			l_FI1->floatingNum = curToughness * (-1.0f);
			l_FI1->specifiedColor = FColor::Red;
			l_FI1->txtLocation = GetActorLocation();
			l_FI1->FinishSpawning(CustomTransform1);

			// 播放血量减少时的受击声音
			//UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSFX, GetActorLocation());

			// 重置盾值相关的变量值
			curToughness = 0.0f;
			shieldDuration = 0;
			maxToughness = 0.0f;
			Tags.Remove(*shieldTag);

			// TBD - 删除盾的特效Actor
		}
	}
	else
	{
		curHP = FMath::Clamp(curHP - dmg, 0.0f, maxHP);

		// 生成红色数字
		FTransform CustomTransform2;
		CustomTransform2.GetLocation() = GetActorLocation();
		CustomTransform2.GetRotation() = FQuat(0, 0, 0, 0);
		CustomTransform2.GetScale3D() = FVector(1, 1, 1);
		AFloatingIndicator* l_FI2 = GetWorld()->SpawnActorDeferred<AFloatingIndicator>(
			FloatingIndicatorClass, CustomTransform2);
		l_FI2->floatingNum = dmg;
		l_FI2->specifiedColor = FColor::Red;
		l_FI2->txtLocation = GetActorLocation();
		l_FI2->FinishSpawning(CustomTransform2);

		// 播放血量减少时的受击声音
		//UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSFX, GetActorLocation());
	}
}

void ATBG_Character_BattlePlayer::BeginPlay()
{
	Super::BeginPlay();
	FString s = DataRow.ToString();
	playerAtr = *(PlayerCharsDT->FindRow<FPlayerCharAttributes>(DataRow, s, true));
	//初始化数据
	InitializeCharData();
	//初始化镜头角度
	float l_springArmYaw = 0.f; 
	//根据位置不同进行偏移
	if (PositionID != -1)
	{
		switch (PositionID)
		{
			case 0 :
				break;
			case 1 :
				l_springArmYaw = -60.f;
				break;
			case 2:
				l_springArmYaw = -55.f;
				break;
			case 3:
				l_springArmYaw = -45.f;
				break;
			case 4:
				l_springArmYaw = -27.f;
				break;
		}
	}
	GetCameraBoom()->SetRelativeRotation(FRotator(0, l_springArmYaw, 0));

	//初始化时间轴
	if (Curve_RotateToTarget)
	{
		FOnTimelineFloat RtTHandler;
		//绑定函数名称，绑定的一定时UFUNCTION,不需要额外标记。
		RtTHandler.BindUFunction(this, FName("TL_RotateToTarget"));
		RotateToTarget_TL.AddInterpFloat(Curve_RotateToTarget, RtTHandler);
	}
	if(Curve_Sliding)
	{
		FOnTimelineFloat SLFHandler;
		SLFHandler.BindUFunction(this, FName("TL_Slide_F"));
		Slide_F_TL.AddInterpFloat(Curve_Sliding, SLFHandler);

		FOnTimelineEvent SLFEventHandler;
		SLFEventHandler.BindUFunction(this, FName("TL_SlideF_Finished"));
		Slide_F_TL.SetTimelineFinishedFunc(SLFEventHandler);

		FOnTimelineFloat SLBHandler;
		SLBHandler.BindUFunction(this, FName("TL_Slide_B"));
		Slide_B_TL.AddInterpFloat(Curve_Sliding, SLBHandler);

		FOnTimelineEvent SLBEventHandler;
		SLBEventHandler.BindUFunction(this, FName("TL_SlideB_Finished"));
		Slide_B_TL.SetTimelineFinishedFunc(SLBEventHandler);
	}
	
}

void ATBG_Character_BattlePlayer::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
	//注册时间轴
	RotateToTarget_TL.TickTimeline(deltaTime);
	Slide_F_TL.TickTimeline(deltaTime);
	Slide_B_TL.TickTimeline(deltaTime);
}
void ATBG_Character_BattlePlayer::Int_SetATK(EAttackType ATKType, int32 AttackCountInOneCycle)
{
	//FString str = FString::SanitizeFloat(AttackCountInOneCycle);
	attackCountIOC = AttackCountInOneCycle;
	bool bCastingBuff= playerAtr.BuffSkillStats.Contains(ATKType);
	if(bCastingBuff)
	{
		buffInfo = *playerAtr.BuffSkillStats.Find(ATKType);
	}
	else
	{
		buffInfo.BuffType = EBuffTypes::BT_EMAX;
		buffInfo.BuffRatio = 0.0f;
	}
	//对单体或者多个对象施加作用
	if (!playerAtr.MultipleTargets.Contains(ATKType)) return;
	if (*(playerAtr.MultipleTargets.Find(ATKType)))
	{
		// 多人对象
		for (auto ArrayElem : currentTargets)
		{
			ICombatInterface* tempInterface = Cast<ICombatInterface>(ArrayElem);
			if (tempInterface)
			{
				float l_hpdmg;
				float l_tdmg;
				CalculateDmg(bCastingBuff, l_hpdmg, l_tdmg);
				tempInterface->Int_HitHandle(this, l_hpdmg, l_tdmg, buffInfo);
			}
		}
	}
	else
	{
		// 单人对象 targetActor
		ICombatInterface* tempInterface = Cast<ICombatInterface>(targetActor);
		if (tempInterface)
		{
			float l_hpdmg;
			float l_tdmg;
			CalculateDmg(bCastingBuff, l_hpdmg, l_tdmg);
			tempInterface->Int_HitHandle(this, l_hpdmg, l_tdmg, buffInfo);
		}
	}

}

void ATBG_Character_BattlePlayer::Int_HitHandle(AActor* causer, float HP_Dmg, float Toughness_Dmg, FBuffInfo buff_Info)
{
	//受击逻辑
	// 受击逻辑
	float l_RealRecievedDmg = HP_Dmg;
	EBuffTypes l_BuffType = buff_Info.BuffType;
	switch (l_BuffType)
	{
	case EBuffTypes::BT_EMAX:

		// 来自敌人攻击时再考虑防御力；增益魔法不考虑防御力
		l_RealRecievedDmg = HP_Dmg * (200.0f / (200.0f + playerAtr.Defend));

		// 处理盾值和血量
		HandleShieldAndHP(l_RealRecievedDmg);

		// 判断播放受击动画还是击败动画（可复活）
		if (curHP <= 0.0f)
		{
			bDead = true;
			PlaySpecifiedAnim("Die");
		}
		else
		{
			PlaySpecifiedAnim("Hit_F");
			// 每受击1次增加能量，设定增加5点
			HandleEP(EAttackType::AT_EMAX, true, 5.0f);
		}
		break;
	case EBuffTypes::BT_Shield:
		// TBD - 盾值不可叠加，但重复被释放可刷新持续时间

		break;
	case EBuffTypes::BT_Heal:
		// TBD - 治疗

		break;
	case EBuffTypes::BT_Resurrection:
		// TBD - 复活逻辑

		break;
	case EBuffTypes::BT_MoveForward:
		// TBD - 行动提前（拉条）


		break;
	default:
		break;
	}

}

void ATBG_Character_BattlePlayer::Int_SetArrowVFX(bool bStartRain)
{
	if (bStartRain)
	{
		for (auto ArrayElem : currentTargets)
		{
			if (ArrayElem)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(
					GetWorld(), ArrowRainVFX, ArrayElem->GetActorLocation());
			}
		}
	}
	else
	{
		// 生成箭Actor
		FVector l_spawnLoc = GetMesh()->GetSocketLocation(FName("socket_wep_l"));
		FRotator l_spawnRot = (targetActor->GetActorLocation() - GetActorLocation()).Rotation();
		GetWorld()->SpawnActor<AActor>(ArrowActorClass, l_spawnLoc, l_spawnRot);
	}
}

