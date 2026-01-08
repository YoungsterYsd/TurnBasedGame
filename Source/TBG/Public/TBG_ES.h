// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "TBG_ES.generated.h"
/**
 * 
 */
UENUM(BlueprintType)
enum class EProgressPhase :uint8
{
	PP_EMAX						UMETA(DisplayName = EMAX),
	PP_A1_PreInitialization		UMETA(DisplayName = A1),
	PP_B1_CalculateActionValue	UMETA(DisplayName = B1),
	PP_B2a_PlayerActionTime		UMETA(DisplayName = B2a),
	PP_B2b_EnemyActionTime		UMETA(DisplayName = B2b),
	PP_B2c_Animating			UMETA(DisplayName = B2c),
	PP_B3_TurnEnd				UMETA(DisplayName = B3),
	PP_A2_BattleEnd				UMETA(DisplayName = A2)
};

