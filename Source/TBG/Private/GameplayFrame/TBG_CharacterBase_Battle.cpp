// YangSida Allright reserved


#include "GameplayFrame/TBG_CharacterBase_Battle.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"


// Sets default values
ATBG_CharacterBase_Battle::ATBG_CharacterBase_Battle()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("Camera Boom");
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bDoCollisionTest = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>("Follow Camera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	LockIcon = CreateDefaultSubobject<UWidgetComponent>("Lock");
	LockIcon->SetupAttachment(RootComponent);
	LockIcon->bHiddenInGame = true;

}

// Called when the game starts or when spawned
void ATBG_CharacterBase_Battle::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATBG_CharacterBase_Battle::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
}

void ATBG_CharacterBase_Battle::Int_GetActionValue(float& actionVal)
{
	actionVal = ActionValue;
}

void ATBG_CharacterBase_Battle::Int_UpdateActionValue(float winnerVal)
{
	//所有角色行动值减去当前行动速度最快的行动值
	ActionValue -= winnerVal;
}

void ATBG_CharacterBase_Battle::Int_RefreshActionValueBySpeed()
{
	//子类覆盖使用
}

void ATBG_CharacterBase_Battle::Int_SetATK(EAttackType ATKType, int32 AttackCountInOneCycle)
{
}

void ATBG_CharacterBase_Battle::Int_HitHandle(AActor* causer, float HP_Dmg, float Toughness_Dmg, FBuffInfo buff_Info)
{
}

void ATBG_CharacterBase_Battle::Int_SetArrowVFX(bool bStartRain)
{
}

void ATBG_CharacterBase_Battle::GetFactionAVAvartar(bool &bPF, float &aV, UTexture2D* &AI)
{
	bPF = bPlayerFaction;
	aV = ActionValue;
	AI = AvatarIcon;
}


