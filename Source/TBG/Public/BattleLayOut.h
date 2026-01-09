// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleLayOut.generated.h"

/**
 * 
 */
class ATBG_BattleManager;
UCLASS()
class TBG_API UBattleLayOut : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void ConstructDeferred(ATBG_BattleManager* BM);

	UFUNCTION(BlueprintImplementableEvent)
	void RefreshActionOrder(const TArray<ACharacter*> &CharRefs);
};
