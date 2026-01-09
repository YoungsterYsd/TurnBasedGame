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

	virtual void Int_RefreshActionValueBySpeed() override;
	EAttackType attackType{ EAttackType::AT_NormalATK };//Ä¬ÈÏÆÕÍ¨¹¥»÷
protected:
	virtual void BeginPlay() override;
};
