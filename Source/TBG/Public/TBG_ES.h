// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "TBG_ES.generated.h"
/**
 * 
 */
class ATBG_Character_BattlePlayer;
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

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	AT_EMAX						UMETA(DisplayName = EMAX),
	AT_NormalATK				UMETA(DisplayName = NormalATK),
	AT_SkillATK					UMETA(DisplayName = SkillATK),
	AT_FollowTK					UMETA(DisplayName = FollowATK),
	AT_Ultimate					UMETA(DisplayName = Ultimate),
	AT_DelayATK_E				UMETA(DisplayName = DelayATK_E)
};

UENUM(BlueprintType)
enum class EBuffTypes : uint8
{
	BT_EMAX						UMETA(DisplayName = EMAX),
	BT_Shield					UMETA(DisplayName = Shield),
	BT_Heal						UMETA(DisplayName = Heal),
	BT_Resurrection				UMETA(DisplayName = Resurrection),
	BT_MoveForward				UMETA(DisplayName = MoveForward)
};

UENUM(BlueprintType)
enum class ECombatType : uint8
{
	CT_EMAX						UMETA(DisplayName = EMAX),
	CT_Physical					UMETA(DisplayName = Physical),
	CT_Fire						UMETA(DisplayName = Fire),
	CT_Ice						UMETA(DisplayName = Ice),
	CT_Lightning				UMETA(DisplayName = Lightning),
	CT_Wind						UMETA(DisplayName = Wind),
	CT_Quantum					UMETA(DisplayName = Quantum),
	CT_Imaginary				UMETA(DisplayName = Imaginary)
};

UENUM(BlueprintType)
enum class EBattleFlags : uint8
{
	BF_EMAX						UMETA(DisplayName = EMAX),
	BF_ContinueBattle			UMETA(DisplayName = ContinueBattle),
	BF_PlayerWin				UMETA(DisplayName = PlayerWin),
	BF_EnemyWin					UMETA(DisplayName = EnemyWin)
};

USTRUCT(BlueprintType)
struct FBuffInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBuffTypes BuffType{ EBuffTypes::BT_EMAX };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BuffRatio = 0.0f;
};

USTRUCT(BlueprintType)
struct FPlayerCharAttributes :public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CharName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECombatType CombatType{ ECombatType ::CT_EMAX};

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float HP = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Attack = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Defend = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float UltimateRatio = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Energy = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* SKM = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UObject> AnimClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CharIcon_Avatar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CharIcon_Banner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CharIcon_Transparent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, UAnimMontage*> Montages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EAttackType, bool> MultipleTargets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EAttackType, bool> MeleeAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EAttackType, FBuffInfo> BuffSkillStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ATBG_Character_BattlePlayer> BattlePlayerClass;
};
USTRUCT(BlueprintType)
struct FEnemyATKInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRadialAction = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRangeAction = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ATK_Distance = 0.0f;
};

USTRUCT(BlueprintType)
struct FEnemyCharAttributes : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CharName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Toughness = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CharIcon_Banner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ECombatType> Weaknesses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HP = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Attack = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) 
	float StunVFXHeight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, UAnimMontage*> AnimMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FEnemyATKInfo> ValidATKStr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, float> Choices;
};

