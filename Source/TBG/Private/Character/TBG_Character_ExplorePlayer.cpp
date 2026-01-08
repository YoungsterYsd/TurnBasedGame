// YangSida Allright reserved


#include "Character/TBG_Character_ExplorePlayer.h"
#include "GameplayFrame\TBG_PlayerControllerBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Character/TBG_Character_ExploreEnemies.h"

ATBG_Character_ExplorePlayer::ATBG_Character_ExplorePlayer()
{
	//玩家开启tick
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	//设置跟随模式
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	//设置移速
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;

}

void ATBG_Character_ExplorePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATBG_Character_ExplorePlayer::BeginPlay()
{
	Super::BeginPlay();

	PC = Cast<ATBG_PlayerControllerBase>(Controller);
	if (PC == nullptr)return;
	//焦点设置为游戏
	PC->SetInputMode(FInputModeGameOnly());

	// 初始化增强型输入系统
	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	// 若初始化失败，则跳出当前函数体
	if (Subsystem == nullptr) return;
	Subsystem->AddMappingContext(IMC_SR, 0);
}

void ATBG_Character_ExplorePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//获取输入组件
	UEnhancedInputComponent* EIComp = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	// 绑定各个按键映射
	EIComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATBG_Character_ExplorePlayer::Move_Triggered);
	EIComp->BindAction(MoveAction, ETriggerEvent::Completed, this, &ATBG_Character_ExplorePlayer::Move_Completed);
	EIComp->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATBG_Character_ExplorePlayer::Look_Triggered);
	EIComp->BindAction(TryAttackAction, ETriggerEvent::Started, this, &ATBG_Character_ExplorePlayer::TryAttack_Started);
}

void ATBG_Character_ExplorePlayer::Move_Triggered(const FInputActionValue& val)
{
	//检测不允许移动的状态
	if (bOpenTeamUI || bATK)return;
	// 按下方向键时触发
	// 按Vector2D形式解析传入的形参
	const FVector2D InputVector = val.Get<FVector2D>();
	float vel_x = InputVector.X;
	float vel_y = InputVector.Y;

	if (PC == nullptr) return;
	// 只关注水平方向Yaw
	// Rotator的形参顺序:Pitch, Yaw, Roll，和蓝图中Rotator顺序不同
	const FRotator GroundRotation(0, PC->GetControlRotation().Yaw, 0);

	// 左右
	const FVector RightDir = FRotationMatrix(GroundRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDir, vel_x);

	// 前后
	const FVector FwdDir = FRotationMatrix(GroundRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(FwdDir, vel_y);
}

void ATBG_Character_ExplorePlayer::Move_Completed(const FInputActionValue& val)
{
	// 松开方向键时触发
}

void ATBG_Character_ExplorePlayer::Look_Triggered(const FInputActionValue& val)
{
	const FVector2D LookVal = val.Get<FVector2D>();
	if (PC == nullptr) return;

	AddControllerYawInput(LookVal.X);
	AddControllerPitchInput(LookVal.Y);
}

void ATBG_Character_ExplorePlayer::TryAttack_Started(const FInputActionValue& val)
{
	//检查是否允许攻击
	if (!bOpenTeamUI && !bATK)
	{
		bATK = true;
		//TODO 切换武器位置
		//TODo 播放蒙太奇
		MeleeDetectEnemy();
	}
}

void ATBG_Character_ExplorePlayer::MeleeDetectEnemy()
{
	//近战攻击逻辑,后续改为GAS实现
	//检测前方
	TArray<FHitResult> OutResults;
	FVector Start = GetActorLocation() + GetActorForwardVector() * 150.f;
	//设定检测人物
	FCollisionObjectQueryParams ObjQueryParams;
	ObjQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	//检测并忽略自身
	FCollisionShape MyShpere = FCollisionShape::MakeSphere(150.f);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	GetWorld()->SweepMultiByObjectType(OutResults, Start, Start, FQuat(0, 0, 0, 0), ObjQueryParams, MyShpere,Params);
	//debug用
	DrawDebugSphere(GetWorld(), Start, 150.f, 15, FColor::Cyan, false,5.0f, 0.f, 4.f);

	//如果碰撞到，则进入战斗
	for (auto Ele : OutResults)
	{
		AActor* DetectedActor = Cast<ATBG_Character_ExploreEnemies>(Ele.GetActor());
		if (DetectedActor != nullptr)
		{
			//TOdo,进入战斗
		}
	}
}
