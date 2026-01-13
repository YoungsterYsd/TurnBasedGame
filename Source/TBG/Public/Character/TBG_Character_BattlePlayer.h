// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayFrame/TBG_CharacterBase_Battle.h"
#include "TBG_ES.h"
#include "Components/TimelineComponent.h"
#include "TBG_Character_BattlePlayer.generated.h"

/**
 * 
 */
class 	UWidgetComponent;
struct	FPlayerCharAttributes;
class	UCurveFloat;
struct	FTimeline;
class	USpringArmComponent;
class	UNiagaraSystem;
UCLASS()
class TBG_API ATBG_Character_BattlePlayer : public ATBG_CharacterBase_Battle
{
	GENERATED_BODY()
public:
	ATBG_Character_BattlePlayer();
	UPROPERTY(EditAnywhere,CateGory = "Runtime")
	int32 PositionID = -1;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, CateGory = "Runtime")
	FPlayerCharAttributes playerAtr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, CateGory = "Presets")
	UDataTable* PlayerCharsDT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, CateGory = "Presets")
	FName DataRow = FName("1");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	UWidgetComponent* MarkedIcon;

	void UpdateLockIcon(bool bHide);
	virtual void Int_RefreshActionValueBySpeed() override;
	void InitializeCharData();
	void PlayAnimationAndTimeline();
	void SetHiddenForPlayer(bool bCustomHidden);
	//动画播放相关
	float PlaySpecifiedAnim(FString str);
	void PlayATKAnimByATKType();
	void AfterPlayingMeleeATKAnim();
	void GeneralPlayerAttackOver();
	void CalculateDmg(bool bBuff, float& hpDmg, float& toughnessDmg);
	void SetDelayedMark(bool bNewVisibility);

	void HandleEP(EAttackType ATKType,bool bDirect,float val);
	UFUNCTION(BlueprintCallable)
	void GetAttributes(float& mHP, float& cHP, float& mEP, float& cEP, float& mT, float& cT);

	UFUNCTION()
	void TL_RotateToTarget(float deltaTime);
	UFUNCTION()
	void TL_Slide_F(float deltaTime);
	UFUNCTION()
	void TL_SlideF_Finished();
	UFUNCTION()
	void TL_Slide_B(float deltaTime);
	UFUNCTION()
	void TL_SlideB_Finished();

	void SingleATK(AActor* target,bool bCounsumeTurn,bool bMelee,EAttackType ATKType);
	void MultipleATK(TArray<AActor*> targets, bool bCounsumeTurn, bool bMelee, EAttackType ATKType);
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float deltaTime)override;
public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,CateGory = "Presets")
	UCurveFloat* Curve_RotateToTarget;//用于角色转向
	FTimeline RotateToTarget_TL;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, CateGory = "Presets")
	UCurveFloat* Curve_Sliding;//冲刺动作

	FTimeline Slide_F_TL;
	FTimeline Slide_B_TL;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, CateGory = "Presets")
	UNiagaraSystem* ArrowRainVFX;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, CateGory = "Presets")
	TSubclassOf<AActor> ArrowActorClass;

	FTimerHandle PlayATKAnimHandler;
	FTimerHandle AfterPlayeringMeleeATKAnimHandle;
	FTimerHandle MeleePlayerEndHandle;

	EAttackType attackType{ EAttackType::AT_NormalATK };//默认普通攻击
	float		maxHP;
	float		curHP;
	float		maxEnergy;
	float		curEnergy;
	float		maxToughness;
	float		curToughness;

	FVector		OringinalLocation;
	FRotator	OriginalRotation;
	bool		ConsumeTurn;
	bool		Melee;
	AActor*		targetActor;
	AActor*		rotateToTarget;
	FVector		targetLocation;
	TArray<AActor*> currentTargets;
	
	float		attackCountIOC;
	FBuffInfo	buffInfo;

	FORCEINLINE USpringArmComponent* GetCameraBoom() { return CameraBoom; }
	virtual void Int_SetATK(EAttackType ATKType, int32 AttackCountInOneCycle) override;
	virtual void Int_HitHandle(AActor* causer, float HP_Dmg, float Toughness_Dmg, FBuffInfo buff_Info) override;

	virtual void Int_SetArrowVFX(bool bStartRain) override;
};
