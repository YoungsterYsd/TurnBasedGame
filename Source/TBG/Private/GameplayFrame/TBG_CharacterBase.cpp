// YangSida Allright reserved


#include "GameplayFrame/TBG_CharacterBase.h"

// Sets default values
ATBG_CharacterBase::ATBG_CharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATBG_CharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATBG_CharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATBG_CharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

