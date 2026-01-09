// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/CombatInterface.h"
#include "TBG_CharacterBase_Battle.generated.h"

UCLASS()
class TBG_API ATBG_CharacterBase_Battle : public ACharacter,public ICombatInterface
{
	GENERATED_BODY()

public:
	ATBG_CharacterBase_Battle();
	bool bDead = false;
	bool bStun = false;
	float ActionValue = 0.f;
	float Distance = 10000.f;


	bool bPlayerFaction = false;
	float actionValue = 0.f;
	UTexture2D* AvatarIcon;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void Int_GetActionValue(float& actionVal) override;
	virtual void Int_UpdateActionValue(float winnerVal) override;
	virtual void Int_RefreshActionValueBySpeed() override;

	UFUNCTION(BlueprintCallable)
	void GetFactionAVAvartar(bool& bPF, float &aV, UTexture2D* &AI);
};
