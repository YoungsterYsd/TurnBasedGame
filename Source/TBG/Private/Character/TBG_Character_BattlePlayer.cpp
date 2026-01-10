// YangSida Allright reserved


#include "Character/TBG_Character_BattlePlayer.h"
#include "Components/WidgetComponent.h"

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
}

void ATBG_Character_BattlePlayer::TL_RotateToTarget(float deltaTime)
{
	//deltaTime 0-1 时间根据Curve而定
	//FString str = FString::SanitizeFloat(deltaTime);
	//玩家转向锁定的Irene方向
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
