// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/CombatInterface.h"
#include "TBG_CharacterBase_Battle.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
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
	UTexture2D* AvatarIcon;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Components")
	TObjectPtr<USpringArmComponent> CameraBoom;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UCameraComponent> FollowCamera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UWidgetComponent> LockIcon;
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime)override;
public:	
	virtual void Int_GetActionValue(float& actionVal) override;
	virtual void Int_UpdateActionValue(float winnerVal) override;
	virtual void Int_RefreshActionValueBySpeed() override;
	virtual void Int_SetATK(EAttackType ATKType, int32 AttackCountInOneCycle) override ;
	virtual void Int_HitHandle(AActor* causer, float HP_Dmg, float Toughness_Dmg, FBuffInfo buff_Info) override;

	UFUNCTION(BlueprintCallable)
	void GetFactionAVAvartar(bool& bPF, float &aV, UTexture2D* &AI);
};
