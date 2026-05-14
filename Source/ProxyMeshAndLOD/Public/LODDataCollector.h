// =====================================================================================
// LODDataCollector.h
// 
// LOD数据采集器 —— 自动记录实验数据并导出CSV
// 
// 功能：
//   1. 定时采集LOD相关数据（FPS、DrawCall、LOD等级、距离等）
//   2. 支持手动触发采集
//   3. 导出CSV文件便于论文分析
// =====================================================================================

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LODDataCollector.generated.h"

/**
 * ALODDataCollector
 * 
 * LOD数据采集Actor，放置在场景中即可自动采集数据
 */
UCLASS()
class PROXYMESHANDLOD_API ALODDataCollector : public AActor
{
	GENERATED_BODY()

public:
	ALODDataCollector();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// =====================================================================================
	// 配置参数
	// =====================================================================================

	/** 采集间隔（秒） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD")
	float SampleInterval = 1.0f;

	/** 是否自动开始采集 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD")
	bool bAutoStart = true;

	/** 最大采集数量（0=无限制） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD")
	int32 MaxSamples = 0;

	// =====================================================================================
	// 蓝图可调用函数
	// =====================================================================================

	/** 开始采集 */
	UFUNCTION(BlueprintCallable, Category = "LOD")
	void StartRecording();

	/** 停止采集 */
	UFUNCTION(BlueprintCallable, Category = "LOD")
	void StopRecording();

	/** 手动记录单帧数据 */
	UFUNCTION(BlueprintCallable, Category = "LOD")
	void RecordSample();

	/** 导出CSV文件 */
	UFUNCTION(BlueprintCallable, Category = "LOD")
	void ExportCSV(FString FileName = "LOD_Data.csv");

	/** 清除所有数据 */
	UFUNCTION(BlueprintCallable, Category = "LOD")
	void ClearData();

	// =====================================================================================
	// 运行时数据
	// =====================================================================================

	/** 当前采集数量 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LOD")
	int32 SampleCount;

	/** 当前FPS */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LOD")
	float CurrentFPS;

	/** 当前DrawCall */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LOD")
	int32 DrawCallCount;

	/** 当前LOD等级 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LOD")
	int32 CurrentLOD;

	/** 摄像机距离 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LOD")
	float CameraDistance;

	/** 屏幕中心距离 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LOD")
	float ScreenCenterDistance;

private:
	// 内部状态
	bool bIsRecording = false;
	float Timer = 0.0f;

	// 数据记录
	struct FLODSample
	{
		float Time;
		float FPS;
		int32 DrawCalls;
		int32 LODLevel;
		float Distance;
		float ScreenCenter;
	};

	TArray<FLODSample> Samples;

	// 内部函数
	void CollectSample();
};
