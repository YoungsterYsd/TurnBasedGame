// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayFrame/TBG_CharacterBase_Battle.h"
#include "TBG_ES.h"
#include "TBG_Character_BattlePlayer.generated.h"

/**
 * 
 */
struct FPlayerCharAttributes;
UCLASS()
class TBG_API ATBG_Character_BattlePlayer : public ATBG_CharacterBase_Battle
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,CateGory = "Runtime")
	int32 PositionID = -1;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, CateGory = "Runtime")
	FPlayerCharAttributes playerAtr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, CateGory = "Presets")
	UDataTable* PlayerCharsDT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, CateGory = "Presets")
	FName DataRow = FName("1");

	virtual void Int_RefreshActionValueBySpeed() override;
protected:
	virtual void BeginPlay() override;
};
