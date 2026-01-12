// YangSida Allright reserved


#include "Character/TBG_Character_BattleEnemies.h"
#include "Character/TBG_Character_BattlePlayer.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HeadBarUI.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Actor/FloatingIndicator.h"

ATBG_Character_BattleEnemies::ATBG_Character_BattleEnemies()
{
	HeadBar = CreateDefaultSubobject<UWidgetComponent>("Head Bar");
	HeadBar->SetupAttachment(RootComponent);
	//HeadBar->bHiddenInGame = true;
}

void ATBG_Character_BattleEnemies::UpdateLockIcon(bool bHide)
{
	LockIcon->bHiddenInGame = bHide;
}

void ATBG_Character_BattleEnemies::Int_RefreshActionValueBySpeed()
{
	ActionValue = Distance/enemyInfo.Speed;
}

void ATBG_Character_BattleEnemies::UpdateHeadBar()
{
	HeadBarUI->UpdateEnemyHeadBar(curHP, curThoughness, maxHP, maxThoughness, Weaknesses);
}

bool ATBG_Character_BattleEnemies::HandleToughness(float receivedT, ECombatType causerType)
{
	bool l_hit = CheckElementATK(causerType);
	if (l_hit)
	{
		curThoughness = curThoughness - receivedT;
		return true;
	}
	else
	{
		return false;
	}
}

void ATBG_Character_BattleEnemies::HandleIndicatorNums(FVector location, float floatingNum)
{
	// 需传入显示的数字，故使用延迟生成
	FTransform CustomTransform;
	CustomTransform.GetLocation() = location;
	CustomTransform.GetRotation() = FQuat(0, 0, 0, 0);
	CustomTransform.GetScale3D() = FVector(1, 1, 1);
	AFloatingIndicator* l_FI = GetWorld()->SpawnActorDeferred<AFloatingIndicator>(
		FloatingIndicatorClass, CustomTransform);
	l_FI->floatingNum = floatingNum;
	l_FI->specifiedColor = FColor::Yellow;
	l_FI->txtLocation = location;
	l_FI->FinishSpawning(CustomTransform);
}

void ATBG_Character_BattleEnemies::HandleFX()
{
	// 播放声音
	//UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSFX, GetActorLocation());

	// 播放受击特效
	NComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitVFX, GetActorLocation());

	// 顿帧
	EnterDialtion();

	// 镜头振动
	PlayerCameraShake();
}

void ATBG_Character_BattleEnemies::EnterDialtion()
{
	// 避免叠加
	if (UGameplayStatics::GetGlobalTimeDilation(GetWorld()) > 0.9f)
	{
		//静态函数中使用：AWorldSettings::SetTimeDilation(0.01f);
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.01f);

		GetWorld()->GetTimerManager().SetTimer(
			dilationHandle,
			this,
			&ATBG_Character_BattleEnemies::ResetDilation,
			0.0005f,
			false
		);
	}
}

void ATBG_Character_BattleEnemies::ResetDilation()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
}

void ATBG_Character_BattleEnemies::PlayerCameraShake()
{
	//TBD
}

bool ATBG_Character_BattleEnemies::CheckElementATK(ECombatType cType)
{
	bool l_dmgToughness = false;
	for (auto ArrElement : Weaknesses)
	{
		if (ArrElement == cType)
		{
			l_dmgToughness = true;
			//省一点点性能，有作用，但不多
			return l_dmgToughness;
		}
	}
	return l_dmgToughness;
}

void ATBG_Character_BattleEnemies::BeginPlay()
{
	Super::BeginPlay(); 

	InitializeCharData();
}

void ATBG_Character_BattleEnemies::Int_HitHandle(AActor* causer, float HP_Dmg, float Toughness_Dmg, FBuffInfo buff_Info)
{
	//受击逻辑
	if (bDead) return;
	dmgCauser = causer;
	//如果元素类型对应弱点，则削减韧性
	ATBG_Character_BattlePlayer* tempPlayerRef = Cast<ATBG_Character_BattlePlayer>(causer);
	if (tempPlayerRef == nullptr)return;
	bool l_hitWeakness = HandleToughness(Toughness_Dmg, tempPlayerRef->playerAtr.CombatType);

	// 在韧性值不为0时，敌人减伤20%
	float l_hpDmgMulti = 1.0f;
	if (l_hitWeakness)
	{
		l_hpDmgMulti = 1.0f;
	}
	else
	{
		l_hpDmgMulti = 0.8f;
	}

	// 在敌人非晕眩状态下，伤害值再降低10%
	float l_hitStunMulti = 0.9f;
	if (bStun)
	{
		l_hitStunMulti = 1.0f;
	}
	else
	{
		l_hitStunMulti = 0.9f;
	}

	// 等级压制造成的额外伤害系数，本案例中持续为1

	receivedHPDmg = HP_Dmg * l_hpDmgMulti * l_hitStunMulti * 1.0f;

	// 扣血
	curHP = curHP - receivedHPDmg;

	// 生成伤害数字Actor
	HandleIndicatorNums(GetActorLocation(), receivedHPDmg);

	// 更新头部状态条
	UpdateHeadBar();

	// 播放受击特效、顿帧、镜头振动、声音
	HandleFX();
}
void ATBG_Character_BattleEnemies::InitializeCharData()
{
	FString s = DataRow.ToString();
	enemyInfo = *(EnemyCharsDT->FindRow<FEnemyCharAttributes>(DataRow, s, true));

	//临时初始化UI相关
	bPlayerFaction = false;
	AvatarIcon = enemyInfo.CharIcon_Banner;
	curThoughness = enemyInfo.Toughness;
	maxThoughness = enemyInfo.Toughness;
	curHP = enemyInfo.HP;
	maxHP = enemyInfo.HP;
	Weaknesses = enemyInfo.Weaknesses;
	animMontages = enemyInfo.AnimMontages;
	validATKStr = enemyInfo.ValidATKStr;
	stunVFXHeight = enemyInfo.StunVFXHeight;
	choices = enemyInfo.Choices;
	originLocaiton = GetActorLocation();
	// 初始化行动值
	Int_RefreshActionValueBySpeed();
	// 初始化头部状态条
	HeadBarUI = Cast<UHeadBarUI>(HeadBar->GetUserWidgetObject());
	if (HeadBarUI)
	{
		UpdateHeadBar();
	}
}
