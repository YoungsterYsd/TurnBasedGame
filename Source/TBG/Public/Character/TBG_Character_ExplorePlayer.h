// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayFrame/TBG_CharacterBase.h"
#include "Interface/ExploreInterface.h"
#include "TBG_Character_ExplorePlayer.generated.h"

/**
 * 
 */
//绑定输入
class ATBG_PlayerControllerBase;
class UInputAction;
class UInputMappingContext;
//镜头控制
class USpringArmComponent;
class UCameraComponent;
class ATBG_Character_BattlePlayer;

class UAnimMontage;

UCLASS()
class TBG_API ATBG_Character_ExplorePlayer : public ATBG_CharacterBase,public IExploreInterface
{
	GENERATED_BODY()
	
public:
	ATBG_Character_ExplorePlayer();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	ATBG_PlayerControllerBase* GetTBGPlayerController(){ return PC;}
	void FinishBattle();
	void ResetBattleBoolean();

public:
	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputMappingContext* IMC_SR;
	bool bOpenTeamUI = false;
	bool bATK = false;
	bool bInBattle = false;
	FTimerHandle ResetBattleBooleanTimerHandle;

	//记录我方站位和敌人信息
	UPROPERTY(EditDefaultsOnly, Category = "Presets")
	TMap<int32, TSubclassOf<ATBG_Character_BattlePlayer>> PlayerTeamInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Presets")
	UAnimMontage* ATKMontage;

protected:
	virtual void BeginPlay() override;

#pragma region InputEvents
	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputAction* MoveAction;

	UFUNCTION()
	void Move_Triggered(const FInputActionValue& val);

	UFUNCTION()
	void Move_Completed(const FInputActionValue& val);

	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputAction* LookAction;

	UFUNCTION()
	void Look_Triggered(const FInputActionValue& val);

	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputAction* TryAttackAction;

	UFUNCTION()
	void TryAttack_Started(const FInputActionValue& val);

#pragma endregion
	//战斗模式切换
	void MeleeDetectEnemy();
	void FindEnemyInfo(AActor* InEnemy);
	virtual void Int_E_ATK(bool bStart) override;

private:
	ATBG_PlayerControllerBase* PC = nullptr;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

};