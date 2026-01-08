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

// Called to bind functionality to input
void ATBG_CharacterBase_Battle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

