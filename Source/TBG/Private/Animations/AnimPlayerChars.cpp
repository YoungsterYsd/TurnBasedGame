// YangSida Allright reserved


#include "Animations/AnimPlayerChars.h"
#include "Character/TBG_Character_BattlePlayer.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Actor/FloatingIndicator.h"

EAttackType const UAnimPlayerChars::GetAttackType()
{
	return attackType;
}

void UAnimPlayerChars::Int_SetUltimateReadyVFX(bool bShow)
{
	// 在大招的准备状态时显示特效
	if (bShow)
	{
		// 检查是否生成了NiagaraComponent
		if (NComp != nullptr)
		{
			// 检查是否已经在播放特效，如果已经播放，则不再重复生成
			if (NComp->IsActive())
			{
				// do nothing.
			}
			else
			{
				SpawnEmitterFunc();
			}
		}
		else
		{
			SpawnEmitterFunc();
		}
	}
	else
	{
		// 取消该特效的激活效果
		if (NComp != nullptr)
		{
			NComp->Deactivate();
		}
	}
} 
void UAnimPlayerChars::SpawnEmitterFunc()
{
	if (UltimateReadyVFX == nullptr) return;

	FVector l_Loc = FVector(
		TryGetPawnOwner()->GetActorLocation().X,
		TryGetPawnOwner()->GetActorLocation().Y,
		TryGetPawnOwner()->GetActorLocation().Z - 102.0f
	);

	// 确认AutoActive = true
	NComp =	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), UltimateReadyVFX, l_Loc);
}


void UAnimPlayerChars::NativeUpdateAnimation(float DeltaSeconds)
{
	ATBG_Character_BattlePlayer* tempPlayerRef = Cast<ATBG_Character_BattlePlayer>(TryGetPawnOwner());
	if (tempPlayerRef)
	{
		attackType = tempPlayerRef->attackType;
	}
}
