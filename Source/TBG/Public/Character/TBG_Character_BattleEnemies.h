// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayFrame/TBG_CharacterBase_Battle.h"
#include "TBG_ES.h"
#include "TBG_Character_BattleEnemies.generated.h"

/**
 * 
 */
class 	UWidgetComponent;
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

	void UpdateLockIcon(bool bHide);
	virtual void Int_RefreshActionValueBySpeed() override;

protected:
	virtual void BeginPlay() override;
};
