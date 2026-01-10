// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayFrame/TBG_CharacterBase_Battle.h"
#include "TBG_ES.h"
#include "TBG_Character_BattlePlayer.generated.h"

/**
 * 
 */
class 	UWidgetComponent;
struct FPlayerCharAttributes;
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

	void SingleATK(AActor* target,bool bCounsumeTurn,bool bMelee,EAttackType ATKType);
	void MultipleATK(TArray<AActor*> targets, bool bCounsumeTurn, bool bMelee, EAttackType ATKType);
protected:
	virtual void BeginPlay() override;
public:
	EAttackType attackType{ EAttackType::AT_NormalATK };//Ä¬ÈÏÆÕÍ¨¹¥»÷
	float maxHP;
	float curHP;
	float maxEnergy;
	float curEnergy;
	FVector OringinalLocation;
	FRotator OriginalRotation;

};
