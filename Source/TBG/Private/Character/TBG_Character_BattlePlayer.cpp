// YangSida Allright reserved


#include "Character/TBG_Character_BattlePlayer.h"
#include "Character\TBG_Character_BattleEnemies.h"
#include "Kismet\KismetMathLibrary.h"
#include "Utilities/CF_SR.h"
#include "TBG_BattleManager.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

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
	curEnergy = maxEnergy;
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
				l_blendTime = 0.6f;
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

	//计时器
	GetWorldTimerManager().SetTimer(AfterPlayeringMeleeATKAnimHandle, this,
		&ATBG_Character_BattlePlayer::AfterPlayingMeleeATKAnim, animTime, false);
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
		&ATBG_Character_BattlePlayer::GeneralPlayerAttackOver, 0.4f, false);
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
