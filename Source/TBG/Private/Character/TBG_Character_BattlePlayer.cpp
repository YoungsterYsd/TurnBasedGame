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
	}
	else
	{
		//TBD 远程逻辑
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
}

void ATBG_Character_BattlePlayer::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
	//注册时间轴
	RotateToTarget_TL.TickTimeline(deltaTime);
} 
