// =====================================================================================
// PerformanceMonitorLibrary.h
// 
// 性能监控库 —— 用于论文实验数据采集
// 
// 功能：
//   1. 获取当前FPS（帧率）
//   2. 获取Draw Call数量
// =====================================================================================

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PerformanceMonitorLibrary.generated.h"

/**
 * UPerformanceMonitorLibrary
 * 
 * 蓝图函数库，提供性能指标监控功能。
 * 用于论文实验：记录LOD切换前后的性能变化。
 */
UCLASS()
class PROXYMESHANDLOD_API UPerformanceMonitorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * 获取当前平均帧率（FPS）
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Performance")
	static float GetCurrentFPS();

	/**
	 * 获取当前Draw Call数量
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Performance")
	static int32 GetDrawCallCount();
};
