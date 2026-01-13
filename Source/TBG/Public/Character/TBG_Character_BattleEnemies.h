// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayFrame/TBG_CharacterBase_Battle.h"
#include "TBG_ES.h"
#include "TBG_Character_BattleEnemies.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyDeath, ATBG_Character_BattleEnemies*, enemyRef, AActor*, causerRef);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyTurnEnd, ATBG_Character_BattleEnemies*, enemyRef);

class	UHeadBarUI;
class 	UWidgetComponent;
class	AFloatingIndicator;
class	UNiagaraSystem;
class	UNiagaraComponent;
class	UCameraShakeBase;
UCLASS()
class TBG_API ATBG_Character_BattleEnemies : public ATBG_CharacterBase_Battle
{
	GENERATED_BODY()
	
public:
	ATBG_Character_BattleEnemies();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
	FEnemyCharAttributes enemyInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	UDataTable* EnemyCharsDT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	FName DataRow = FName("1");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	UWidgetComponent* HeadBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	TSubclassOf<AFloatingIndicator> FloatingIndicatorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	UNiagaraSystem* HitVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Presets")
	UNiagaraSystem* StunVFX;

	UNiagaraComponent* StunVFXComp;

	UNiagaraComponent* NComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Presets")
	TSubclassOf<UCameraShakeBase> normalCS;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Presets")
	TSubclassOf<UCameraShakeBase> skillCS;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Presets")
	TSubclassOf<UCameraShakeBase> followCS;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Presets")
	TSubclassOf<UCameraShakeBase> ultimateCS;


	UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
	FOnEnemyDeath OnEnemyDeath;

	UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
	FOnEnemyTurnEnd OnEnemyTurnEnd;

	void UpdateLockIcon(bool bHide);
	void InitializeCharData();
	void UpdateHeadBar();
	bool HandleToughness(float receivedT,ECombatType causerType);
	void HandleIndicatorNums(FVector location, float floatingNum);
	void HandleFX();
	void EnterDialtion();
	void ResetDilation();
	void PlayerCameraShake();

	UFUNCTION(BlueprintCallable)
	float PlaySpecificAnim(const FString& animKey);

	UFUNCTION(BlueprintCallable)
	void EndEnemyTurnFromBP(ATBG_Character_BattleEnemies* activeActorRef);

	void EnterStun(int32 delayTurns);
	void PlayStunVFX();
	void SetDelayedTarget(bool delay, ATBG_Character_BattlePlayer* target);
	void RecoverFromStun();
	EAttackType ActionDecision(const TArray<ATBG_Character_BattlePlayer*> playerRef);
	FString RandomActionByRatio();

	UFUNCTION(BlueprintImplementableEvent)
	void ExtraActionWhenStun(bool bEnter);

	UFUNCTION(BlueprintImplementableEvent)
	void SingleATK(ATBG_Character_BattlePlayer* target);

	UFUNCTION(BlueprintImplementableEvent)
	void RadialATK(const TArray<ATBG_Character_BattlePlayer*> &targets);

	bool CheckElementATK(ECombatType cType);

	virtual void Int_RefreshActionValueBySpeed() override;
	virtual void Int_HitHandle(AActor* causer, float HP_Dmg, float Toughness_Dmg, FBuffInfo buff_Info) override;
	virtual void Int_SetATK(EAttackType ATKType, int32 AttackCountInOneCycle) override;

	float curThoughness;
	float maxThoughness;
	float curHP;
	float maxHP;
	float totalATK;
	float stunVFXHeight;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVector originLocaiton;
	TArray<ECombatType> Weaknesses;
	TMap<FString, UAnimMontage*> animMontages;
	TMap<FString, FEnemyATKInfo> validATKStr;
	TMap<FString, float> choices;
	UHeadBarUI* HeadBarUI;

	AActor* dmgCauser;
	float receivedHPDmg;
	int32 recoverFromStunTurns = 0;
	bool bDelayed_ATK = false;
	ATBG_Character_BattlePlayer* delayedTarget;
	//用于攻击行为的选择
	bool bRadialATK;
	ATBG_Character_BattlePlayer* currentTarget;
	TArray<ATBG_Character_BattlePlayer*> currentTargetsArr;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "BP")
	FString actionAnimKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "BP")
	bool bRangeATK;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "BP")
	int32 ATKDistance;


	FTimerHandle dilationHandle;

protected:
	virtual void BeginPlay() override;


};
