// =====================================================================================
// LODDataCollector.cpp
// 
// LOD数据采集器实现
// =====================================================================================

#include "LODDataCollector.h"
#include "ProxyMeshGenerator.h"
#include "PerformanceMonitorLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "HAL/FileManager.h"

ALODDataCollector::ALODDataCollector()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALODDataCollector::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoStart)
	{
		StartRecording();
	}
}

void ALODDataCollector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsRecording) return;

	Timer += DeltaTime;
	if (Timer >= SampleInterval)
	{
		Timer = 0.0f;
		CollectSample();

		// 检查是否达到最大采样数
		if (MaxSamples > 0 && SampleCount >= MaxSamples)
		{
			StopRecording();
		}
	}
}

void ALODDataCollector::StartRecording()
{
	bIsRecording = true;
	Timer = SampleInterval; // 立即采集第一次
}

void ALODDataCollector::StopRecording()
{
	bIsRecording = false;
}

void ALODDataCollector::RecordSample()
{
	CollectSample();
}

void ALODDataCollector::CollectSample()
{
	// 获取玩家控制器
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	APlayerCameraManager* CameraManager = PC->PlayerCameraManager;
	if (!CameraManager) return;

	// 采集性能数据
	CurrentFPS = UPerformanceMonitorLibrary::GetCurrentFPS();
	DrawCallCount = UPerformanceMonitorLibrary::GetDrawCallCount();

	// 采集LOD数据
	FVector ActorLocation = GetActorLocation();
	FVector CameraLocation = CameraManager->GetCameraLocation();

	CameraDistance = FVector::Dist(ActorLocation, CameraLocation);
	ScreenCenterDistance = UProxyMeshGenerator::GetScreenCenterDistance(ActorLocation, PC);
	CurrentLOD = UProxyMeshGenerator::GetLODLevel(CameraDistance, ScreenCenterDistance);

	// 保存数据
	FLODSample Sample;
	Sample.Time = GetGameTimeSinceCreation();
	Sample.FPS = CurrentFPS;
	Sample.DrawCalls = DrawCallCount;
	Sample.LODLevel = CurrentLOD;
	Sample.Distance = CameraDistance;
	Sample.ScreenCenter = ScreenCenterDistance;

	Samples.Add(Sample);
	SampleCount = Samples.Num();
}

void ALODDataCollector::ExportCSV(FString FileName)
{
	if (Samples.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("LODDataCollector: No data to export"));
		return;
	}

	// 构建CSV内容
	FString CSVContent = TEXT("Time,FPS,DrawCalls,LODLevel,Distance,ScreenCenter\n");

	for (const FLODSample& Sample : Samples)
	{
		CSVContent += FString::Printf(TEXT("%.2f,%.1f,%d,%d,%.0f,%.3f\n"),
			Sample.Time,
			Sample.FPS,
			Sample.DrawCalls,
			Sample.LODLevel,
			Sample.Distance,
			Sample.ScreenCenter);
	}

	// 保存到文件
	FString SavePath = FPaths::ProjectSavedDir() + TEXT("LODTest/");
	IFileManager::Get().MakeDirectory(*SavePath, true);

	FString FullPath = SavePath + FileName;
	FFileHelper::SaveStringToFile(CSVContent, *FullPath);

	UE_LOG(LogTemp, Log, TEXT("LODDataCollector: Exported to %s"), *FullPath);
}

void ALODDataCollector::ClearData()
{
	Samples.Empty();
	SampleCount = 0;
}
