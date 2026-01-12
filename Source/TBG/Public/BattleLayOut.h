// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleLayOut.generated.h"

/**
 * 
 */
class ATBG_BattleManager;
class ATBG_Character_BattlePlayer;
class ATBG_Character_BattleEnemies;
UCLASS()
class TBG_API UBattleLayOut : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void ConstructDeferred(ATBG_BattleManager* BM);

	UFUNCTION(BlueprintImplementableEvent)
	void RefreshActionOrder(const TArray<ACharacter*> &CharRefs);

	UFUNCTION(BlueprintImplementableEvent)
	void HandleStatsPanelAnimating(ATBG_Character_BattlePlayer* playerRef,bool bIncrease);

	UFUNCTION(BlueprintImplementableEvent)
	void SwitchATKMode(EAttackType ATKType);
	UFUNCTION(BlueprintImplementableEvent)
	void HandlePhaseHintFromCpp(ATBG_Character_BattlePlayer* playerRef, EAttackType ATKType);

	UFUNCTION(BlueprintImplementableEvent)
	void HandlePhaseHintForEnemyTurn(ATBG_Character_BattleEnemies* enemyRef,bool bfixedCamera, EAttackType ATKType);

	UFUNCTION(BlueprintImplementableEvent)
	void HideATKButtons();
	UFUNCTION(BlueprintImplementableEvent)
	void RefreshUltimateOrder(const TArray<ATBG_Character_BattlePlayer*>& playerCharRefs);
	UFUNCTION(BlueprintImplementableEvent)
	void BattleOverHint();
};
