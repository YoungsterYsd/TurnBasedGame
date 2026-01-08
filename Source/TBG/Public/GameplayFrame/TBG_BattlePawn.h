// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TBG_BattlePawn.generated.h"

class ATBG_PlayerControllerBase;
UCLASS()
class TBG_API ATBG_BattlePawn : public APawn
{
	GENERATED_BODY()

public:
	
	ATBG_BattlePawn();
	UFUNCTION(BlueprintCallable)
	void TryNormalATK();
	ATBG_PlayerControllerBase* PC = nullptr;
protected:

	virtual void BeginPlay() override;
	virtual void Destroyed() override;

};
