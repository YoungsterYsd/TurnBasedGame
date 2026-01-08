// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayFrame/TBG_CharacterBase.h"
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

UCLASS()
class TBG_API ATBG_Character_ExplorePlayer : public ATBG_CharacterBase
{
	GENERATED_BODY()
	
public:
	ATBG_Character_ExplorePlayer();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	ATBG_PlayerControllerBase* GetTBGPlayerController(){ return PC;}
public:
	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputMappingContext* IMC_SR;
	bool bOpenTeamUI;
	bool bATK;
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
private:
	ATBG_PlayerControllerBase* PC = nullptr;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

};
