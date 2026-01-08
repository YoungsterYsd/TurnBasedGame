// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleLayOut.generated.h"

/**
 * 
 */
class UTBG_BattleManager;
UCLASS()
class TBG_API UBattleLayOut : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void ConstructDeferred(UTBG_BattleManager* BM);
	
};
