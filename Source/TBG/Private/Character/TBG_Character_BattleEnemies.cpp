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
#include "Camera/CameraShakeBase.h"
#include "Kismet\KismetMathLibrary.h"
#include "Utilities\CF_SR.h"
#include "TBG_BattleManager.h"
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
	// 根据攻击的种类不同，镜头振动的效果也不同
	ATBG_Character_BattlePlayer*  tempPlayerRef = Cast<ATBG_Character_BattlePlayer>(dmgCauser);
	if (tempPlayerRef == nullptr) return;
	EAttackType l_at = tempPlayerRef->attackType;

	TSubclassOf<UCameraShakeBase> l_CSB = nullptr;
	switch (l_at)
	{
	case EAttackType::AT_EMAX:
		l_CSB = nullptr;
		break;
	case EAttackType::AT_NormalATK:
		l_CSB = normalCS;
		break;
	case EAttackType::AT_SkillATK:
		l_CSB = skillCS;
		break;
	case EAttackType::AT_FollowTK:
		l_CSB = followCS;
		break;
	case EAttackType::AT_Ultimate:
		l_CSB = ultimateCS;
		break;
	case EAttackType::AT_DelayATK_E:
		l_CSB = nullptr;
		break;
	default:
		l_CSB = nullptr;
		break;
	}

	if (l_CSB == nullptr) return;

	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayWorldCameraShake(
		GetWorld(),
		l_CSB,
		GetActorLocation(),
		0.0f,
		4000.0f,
		0.0f,
		false
	);
}

float ATBG_Character_BattleEnemies::PlaySpecificAnim(const FString& animKey)
{
	// 如果未找到Key，则不播放
	if (!animMontages.Contains(animKey)) return 0.0f;

	// 如果找到Key， 但Value无效，则不播放
	if (*(animMontages.Find(animKey)) == nullptr) return 0.0f;

	float l_animTime = PlayAnimMontage(*(animMontages.Find(animKey)));

	return l_animTime;
}

void ATBG_Character_BattleEnemies::EndEnemyTurnFromBP(ATBG_Character_BattleEnemies* activeActorRef)
{
	OnEnemyTurnEnd.Broadcast(activeActorRef);
}

void ATBG_Character_BattleEnemies::EnterStun(int32 delayTurns)
{
	// 晕眩持续delayTurns回合，默认1回合

	bStun = true;

	PlaySpecificAnim("Stun");

	// 晕眩即破盾，基础效果推条25%，即行动推迟25%；即ActionValue增加
	// 进阶破盾效果暂不考虑（如不同属性击破后出现debuff）
	ActionValue = ActionValue + (10000 / enemyInfo.Speed) * 0.25;
	
	// 设置晕眩持续的回合数
	recoverFromStunTurns = delayTurns;

	// 播放晕眩特效
	PlayStunVFX();

	// 配合BOSS的晕眩动画，设置自定义事件供重写
	ExtraActionWhenStun(true);

	// 如果BOSS已经用延迟攻击，那么取消该延迟攻击
	if (bDelayed_ATK)
	{
		SetDelayedTarget(false, delayedTarget);
	}
}

void ATBG_Character_BattleEnemies::PlayStunVFX()
{
	// 若已经有晕眩特效，忽略以下逻辑
	if (StunVFXComp != nullptr) return;

	FVector l_loc = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + stunVFXHeight);
	StunVFXComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), StunVFX, FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + stunVFXHeight));
}

void ATBG_Character_BattleEnemies::SetDelayedTarget(bool delay, ATBG_Character_BattlePlayer* target)
{
	// BOSS战时需设置
	bDelayed_ATK = delay;
	if (target == nullptr) return;
	// 设置玩家角色的锁定图标
	target->SetDelayedMark(bDelayed_ATK);

	// 根据锁定的布尔值，重置延迟锁定目标
	if (bDelayed_ATK)
	{
		delayedTarget = target;
	}
	else
	{
		delayedTarget = nullptr;
	}
}

void ATBG_Character_BattleEnemies::RecoverFromStun()
{
	// 消耗一回合，减少一次
	recoverFromStunTurns = recoverFromStunTurns - 1;
	if (recoverFromStunTurns > 0) return;

	// 回复正常
	StopAnimMontage();

	// 重置相关变量的值
	recoverFromStunTurns = 0;
	curThoughness = maxThoughness;
	UpdateHeadBar();
	bStun = false;

	ExtraActionWhenStun(false);

	// 销毁晕眩特效
	if (StunVFXComp != nullptr)
	{
		StunVFXComp->DestroyComponent();
		//StunVFXComp = nullptr;
	}
}

EAttackType ATBG_Character_BattleEnemies::ActionDecision(const TArray<ATBG_Character_BattlePlayer*> playersRef)
{
	
	TArray<ATBG_Character_BattlePlayer*> l_playerRef = playersRef;
	if (delayedTarget != nullptr)
	{
		if (!delayedTarget->bDead && bDelayed_ATK)
		{
			// 优先延迟攻击
			actionAnimKey = "DelayedATK";
		}
		else
		{
			actionAnimKey = RandomActionByRatio();
		}
	}
	else
	{
		actionAnimKey = RandomActionByRatio();
	}

	if (actionAnimKey == "None") return EAttackType::AT_EMAX;

	// 是否范围攻击？远程攻击？攻击距离是？
	if (!validATKStr.Contains(actionAnimKey)) return EAttackType::AT_EMAX;
	bRadialATK = validATKStr.Find(actionAnimKey)->bRadialAction;
	bRangeATK = validATKStr.Find(actionAnimKey)->bRangeAction;
	ATKDistance = int32(validATKStr.Find(actionAnimKey)->ATK_Distance);

	if (bRadialATK)
	{
		// TBD - 范围攻击
		RadialATK(l_playerRef);

		// 如果切换视角，切换至受击的玩家角色视角
		if (UCF_SR::Flib_GetBM()->bBOSSFight)
		{
			return EAttackType::AT_SkillATK;
		}
		else
		{
			UCF_SR::Flib_GetBM()->SwitchAndHideOtherPlayerChars(true, l_playerRef[0]);
			return EAttackType::AT_SkillATK;
		}
	}
	else
	{
		if (bDelayed_ATK)
		{
			// TBD - 延迟攻击
			SingleATK(delayedTarget);

			// 延迟攻击已结束，重置相关bool
			SetDelayedTarget(false, delayedTarget);

			// 如果切换视角，切换至受击的玩家角色视角
			if (UCF_SR::Flib_GetBM()->bBOSSFight)
			{
				return EAttackType::AT_DelayATK_E;
			}
			else
			{
				UCF_SR::Flib_GetBM()->SwitchAndHideOtherPlayerChars(true, delayedTarget);
				return EAttackType::AT_DelayATK_E;
			}

			return EAttackType::AT_DelayATK_E;
		}
		else
		{
			// 普通单体攻击时，优先攻击带盾（嘲讽效果）的玩家角色
			// 具体表现为查找特定tag
			ATBG_Character_BattlePlayer* l_ShieldPlayerRef = nullptr;

			for (auto ArrayElem : l_playerRef)
			{
				if (ArrayElem->ActorHasTag("tag_shield"))
				{
					l_ShieldPlayerRef = ArrayElem;
				}
			}

			if (l_ShieldPlayerRef != nullptr)
			{
				// TBD - 有套盾对象
				SingleATK(l_ShieldPlayerRef);

				// 如果切换视角，切换至受击的玩家角色视角
				if (UCF_SR::Flib_GetBM()->bBOSSFight)
				{
					return EAttackType::AT_NormalATK;
				}
				else
				{
					UCF_SR::Flib_GetBM()->SwitchAndHideOtherPlayerChars(true, l_ShieldPlayerRef);
					return EAttackType::AT_NormalATK;
				}
			}
			else
			{
				// 无套盾对象
				ATBG_Character_BattlePlayer* l_TargetActor = nullptr;
				// 打乱数组l_playerRef后取第一个，相当于随机取一个
				l_TargetActor = l_playerRef[FMath::RandRange(0, (l_playerRef.Num() - 1))];

				if (l_TargetActor == nullptr) return EAttackType::AT_EMAX;

				// 单体攻击
				SingleATK(l_TargetActor);

				// 如果切换视角，切换至受击的玩家角色视角
				if (UCF_SR::Flib_GetBM()->bBOSSFight)
				{
					return EAttackType::AT_NormalATK;
				}
				else
				{
					UCF_SR::Flib_GetBM()->SwitchAndHideOtherPlayerChars(true, l_TargetActor);
					return EAttackType::AT_NormalATK;
				}
			}
		}
	}
	
	return EAttackType();
}

FString ATBG_Character_BattleEnemies::RandomActionByRatio()
{
	// 按照概率选择攻击招式（在DataTable中设置概率，至少有一个攻击应为1.0概率，兜底）
	TArray<FString> l_StrChoices;
	choices.GenerateKeyArray(l_StrChoices);
	for (auto ArrayElem : l_StrChoices)
	{
		//必然存在，故无需用Contain检查；若为空，则报错
		if (!choices.Contains(ArrayElem)) return "None";

		bool l_selected = UKismetMathLibrary::RandomBoolWithWeight(*(choices.Find(ArrayElem)));
		if (l_selected)
		{
			// 立刻跳出
			return ArrayElem;
		}
	}

	return "None";
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
	// 是否死亡
	if (curHP <= 0)
	{
		//  - 死亡逻辑
		bDead = true;
		PlaySpecificAnim("Die");
		HeadBar->SetVisibility(false);

		ExtraActionWhenStun(true);
		//组件有效则删除
		if (StunVFXComp != nullptr)
		{
			StunVFXComp->DestroyComponent();
			StunVFXComp = nullptr;
		}
		//死亡事件派发
		OnEnemyDeath.Broadcast(this, dmgCauser);
		//OnEnemyTurnEnd.Broadcast(this);
	}
	else
	{
		// 是否晕眩
		if (curThoughness <= 0)
		{
			// 若已经进入晕眩状态，则不再重复播放晕眩动画
			if (bStun) return;
			EnterStun(1);
		}
		else
		{
			// 普通受击
			FString str;
			bool bRandomBool = FMath::RandHelper(100) > 50;
			if (bRandomBool)
			{
				str = "Hit1";
			}
			else
			{
				str = "Hit2";
			}
			PlaySpecificAnim(str);
		}
	}
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
