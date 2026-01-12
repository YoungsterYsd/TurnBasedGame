// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingIndicator.generated.h"

UCLASS()
class TBG_API AFloatingIndicator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingIndicator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	float floatingNum = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	FColor specifiedColor = FColor::Blue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	FVector txtLocation;

};
