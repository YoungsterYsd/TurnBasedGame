// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TBG_PlayerControllerBase.generated.h"

/**
 * 
 */
class UBattleLayOut;
class UUserWidget;
class UTBG_BattleManager;
UCLASS()
class TBG_API ATBG_PlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	TSubclassOf<UUserWidget> BattleLayoutClassRef;
	UBattleLayOut* BattleLayOut;
	void LoadBattleUI(UTBG_BattleManager* BM);
};
