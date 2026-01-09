// YangSida Allright reserved


#include "GameplayFrame/TBG_CharacterBase_Battle.h"

// Sets default values
ATBG_CharacterBase_Battle::ATBG_CharacterBase_Battle()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATBG_CharacterBase_Battle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATBG_CharacterBase_Battle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

void ATBG_CharacterBase_Battle::GetFactionAVAvartar(bool &bPF, float &aV, UTexture2D* &AI)
{
	bPF = bPlayerFaction;
	aV = actionValue;
	AI = AvatarIcon;
}


