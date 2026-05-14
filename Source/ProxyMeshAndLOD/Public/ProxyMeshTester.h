// =====================================================================================
// ProxyMeshTester.h
// 
// 自动LOD应用组件 —— 挂载到Actor上自动计算并应用LOD等级
// 
// 功能：
//   1. 定时检测物体到摄像机的距离
//   2. 计算多因素LOD决策（摄像机距离 + 屏幕中心距离）
//   3. 自动设置静态网格体的LOD level
//   4. 支持手动触发LOD更新
// =====================================================================================

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProxyMeshTester.generated.h"

/**
 * ULODApplicatorComponent
 * 
 * 自动LOD应用组件，挂载到需要LOD控制的Actor上
 * 定期检测并自动切换LOD等级
 * 
 * 使用方法：
 *   1. 在蓝图或C++中创建Actor
 *   2. 添加此组件
 *   3. 指定TargetComponent（静态网格体组件）
 *   4. 设置PlayerController（用于屏幕位置计算）
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROXYMESHANDLOD_API ULODApplicatorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULODApplicatorComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// =====================================================================================
	// 配置参数
	// =====================================================================================

	/** 是否启用自动LOD应用 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD")
	bool bAutoApply = true;

	/** LOD更新间隔（秒） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD")
	float UpdateInterval = 0.1f;

	/** 目标静态网格体组件（需要应用LOD的组件） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD")
	UStaticMeshComponent* TargetStaticMesh;

	/** 玩家控制器（用于计算屏幕位置，为空则自动获取） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD")
	APlayerController* PlayerController;

	/** 最大LOD等级（0=最高精度） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD")
	int32 MaxLODLevel = 4;

	// =====================================================================================
	// 蓝图可调用函数
	// =====================================================================================

	/** 手动触发LOD更新 */
	UFUNCTION(BlueprintCallable, Category = "LOD")
	void UpdateLOD();

	/** 设置目标静态网格体 */
	UFUNCTION(BlueprintCallable, Category = "LOD")
	void SetTargetMesh(UStaticMeshComponent* NewTarget);

	/** 启用/禁用自动LOD */
	UFUNCTION(BlueprintCallable, Category = "LOD")
	void SetAutoApply(bool bEnable);

	// =====================================================================================
	// 运行时数据（可在蓝图中读取）
	// =====================================================================================

	/** 当前LOD等级 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LOD")
	int32 CurrentLODLevel;

	/** 摄像机距离 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LOD")
	float CameraDistance;

	/** 屏幕中心距离 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LOD")
	float ScreenCenterDistance;

	/** 是否成功应用LOD */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LOD")
	bool bApplied;

private:
	// 定时器
	float Timer = 0.0f;

	// 内部函数
	void CheckAndApplyLOD();
	APlayerController* GetPlayerController();
};
