// YangSida Allright reserved


#include "Actor/FloatingIndicator.h"

// Sets default values
AFloatingIndicator::AFloatingIndicator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AFloatingIndicator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFloatingIndicator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

