// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TBG_PlayerControllerBase.generated.h"

/**
 * 
 */
class ATBG_BattlePawn;
UCLASS()
class TBG_API ATBG_PlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	TSubclassOf<ATBG_BattlePawn> BattlePawnClass;

};
