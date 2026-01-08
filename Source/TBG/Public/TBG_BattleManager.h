// YangSida Allright reserved

#pragma once

#include "CoreMinimal.h"

#include "Subsystems/GameInstanceSubsystem.h"
#include "TBG_BattleManager.generated.h"

/**
 * 
 */
 //参考教程使用自定义单例进行制作，但UE提供更便捷的Subsystem系统，更换实现方式。
class ATBG_Character_ExploreEnemies;
class ATBG_Character_ExplorePlayer;
UCLASS()
class TBG_API UTBG_BattleManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }
	virtual void Initialize(FSubsystemCollectionBase& Collection)override;
	virtual void Deinitialize()override;
	void InitBattle(ATBG_Character_ExploreEnemies* EnemyRef, ATBG_Character_ExplorePlayer* PlayerRef);

	
};
