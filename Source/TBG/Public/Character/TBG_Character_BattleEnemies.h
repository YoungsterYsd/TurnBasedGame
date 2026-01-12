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
	float PlaySpecificAnim(const FString& animKey);
	void EnterStun(int32 delayTurns);
	void PlayStunVFX();
	void SetDelayedTarget(bool delay, ATBG_Character_BattlePlayer* target);
	void RecoverFromStun();

	UFUNCTION(BlueprintImplementableEvent)
	void ExtraActionWhenStun(bool bEnter);

	bool CheckElementATK(ECombatType cType);

	virtual void Int_RefreshActionValueBySpeed() override;
	virtual void Int_HitHandle(AActor* causer, float HP_Dmg, float Toughness_Dmg, FBuffInfo buff_Info) override;

	float curThoughness;
	float maxThoughness;
	float curHP;
	float maxHP;
	float stunVFXHeight;
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

	FTimerHandle dilationHandle;

protected:
	virtual void BeginPlay() override;


};
