// =====================================================================================
// ProxyMeshTester.cpp
// 
// 自动LOD应用组件 —— 实现
// 
// 注意：本组件只计算多因素LOD等级并暴露给蓝图
// 实际应用到静态网格体的LOD切换由蓝图根据CurrentLODLevel实现
// 这样可以兼容不同UE版本的API差异
// =====================================================================================

#include "ProxyMeshTester.h"
#include "ProxyMeshGenerator.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/StaticMeshComponent.h"

// =====================================================================================
// 构造函数
// =====================================================================================
ULODApplicatorComponent::ULODApplicatorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

// =====================================================================================
// BeginPlay
// =====================================================================================
void ULODApplicatorComponent::BeginPlay()
{
	Super::BeginPlay();

	// 如果没有指定TargetMesh，尝试从Owner获取第一个StaticMeshComponent
	if (!TargetStaticMesh)
	{
		if (AActor* Owner = GetOwner())
		{
			TargetStaticMesh = Owner->FindComponentByClass<UStaticMeshComponent>();
		}
	}
}

// =====================================================================================
// TickComponent
// =====================================================================================
void ULODApplicatorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bAutoApply)
	{
		Timer += DeltaTime;
		if (Timer >= UpdateInterval)
		{
			Timer = 0.0f;
			UpdateLOD();
		}
	}
}

// =====================================================================================
// UpdateLOD - 更新LOD等级计算
// =====================================================================================
void ULODApplicatorComponent::UpdateLOD()
{
	if (!TargetStaticMesh)
	{
		// 尝试重新获取
		if (AActor* Owner = GetOwner())
		{
			TargetStaticMesh = Owner->FindComponentByClass<UStaticMeshComponent>();
		}
		
		if (!TargetStaticMesh)
		{
			bApplied = false;
			return;
		}
	}

	// 获取Owner的位置
	FVector ActorLocation = GetOwner()->GetActorLocation();

	// 获取玩家控制器
	APlayerController* PC = GetPlayerController();
	if (!PC)
	{
		bApplied = false;
		return;
	}

	// 计算多因素LOD等级（核心算法）
	int32 LODLevel = UProxyMeshGenerator::CalculateLODLevel(ActorLocation, PC);

	// 限制在MaxLODLevel范围内
	LODLevel = FMath::Clamp(LODLevel, 0, MaxLODLevel);

	// 记录运行时数据（供蓝图读取）
	CameraDistance = UProxyMeshGenerator::GetCameraDistance(ActorLocation, PC->PlayerCameraManager);
	ScreenCenterDistance = UProxyMeshGenerator::GetScreenCenterDistance(ActorLocation, PC);
	CurrentLODLevel = LODLevel;

	// 实际应用到静态网格体的LOD切换
	if (TargetStaticMesh)
	{
		// SetForcedLodModel 参数范围：[0, MaxLODLevel]
		// 0 = 退出强制LOD，使用自动计算
		// 1 = 最高精度，Max = 最低精度
		// 注意：UE中LOD索引从1开始，所以需要+1
		TargetStaticMesh->SetForcedLodModel(LODLevel + 1);
		
		// 刷新渲染状态，确保LOD切换立即生效
		TargetStaticMesh->MarkRenderStateDirty();
		
		bApplied = true;
	}
	else
	{
		bApplied = false;
	}
}

// =====================================================================================
// GetPlayerController
// =====================================================================================
APlayerController* ULODApplicatorComponent::GetPlayerController()
{
	// 如果已指定，直接返回
	if (PlayerController)
	{
		return PlayerController;
	}

	// 自动获取第一个本地玩家控制器
	UWorld* World = GetWorld();
	if (World)
	{
		for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
		{
			if (APlayerController* PC = It->Get())
			{
				if (PC->IsLocalPlayerController())
				{
					return PC;
				}
			}
		}
	}

	return nullptr;
}

// =====================================================================================
// SetTargetMesh
// =====================================================================================
void ULODApplicatorComponent::SetTargetMesh(UStaticMeshComponent* NewTarget)
{
	TargetStaticMesh = NewTarget;
}

// =====================================================================================
// SetAutoApply
// =====================================================================================
void ULODApplicatorComponent::SetAutoApply(bool bEnable)
{
	bAutoApply = bEnable;
}
