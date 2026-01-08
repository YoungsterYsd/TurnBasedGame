// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayFrame/TBG_CharacterBase.h"
#include "TBG_Character_ExploreEnemies.generated.h"

/**
 * 
 */
class ATBG_Character_ExplorePlayer;
UCLASS()
class TBG_API ATBG_Character_ExploreEnemies : public ATBG_CharacterBase
{
	GENERATED_BODY()
public:
	ATBG_Character_ExploreEnemies();
	virtual void BeginPlay() override;
	void EnterBattleThroughBM(ATBG_Character_ExplorePlayer* playerRef);
	
};
