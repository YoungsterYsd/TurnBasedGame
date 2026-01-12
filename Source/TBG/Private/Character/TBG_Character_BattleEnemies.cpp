// YangSida Allright reserved


#include "Character/TBG_Character_BattleEnemies.h"
#include "Components/WidgetComponent.h"
#include "UI/HeadBarUI.h"

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

void ATBG_Character_BattleEnemies::BeginPlay()
{
	Super::BeginPlay(); 

	InitializeCharData();
}

void ATBG_Character_BattleEnemies::Int_HitHandle(AActor* causer, float HP_Dmg, float Toughness_Dmg, FBuffInfo buff_Info)
{
	//受击逻辑
	if (bDead) return;
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
