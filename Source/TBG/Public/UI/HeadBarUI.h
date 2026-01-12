// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HeadBarUI.generated.h"

/**
 * 
 */
UCLASS()
class TBG_API UHeadBarUI : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateEnemyHeadBar(float cHP, float cT, float mHP, float mT, const TArray<ECombatType>& weaknesses);
};
