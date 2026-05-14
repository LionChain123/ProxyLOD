// =====================================================================================
// ProxyMeshGenerator.cpp
// 
// 多因素LOD决策管理器 —— 核心算法实现
// 
// 综合两个因素决定物体的细节层级（LOD）：
//   因素1 —— 摄像机距离（Distance）：物体离摄像机越远，LOD等级越高
//   因素2 —— 屏幕中心距离（ScreenCenterDistance）：物体在屏幕上越偏离中心
// =====================================================================================

#include "ProxyMeshGenerator.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"

// =====================================================================================
// GetLODLevel —— 多因素LOD等级计算
// 
// 两个因素：
//   因素1：摄像机距离（Distance）—— 权重 70%
//     物体离摄像机越远，所需细节越少，LOD等级越高
//     映射范围：0 → LOD 0（最近），10000 → LOD 4（最远）
// 
//   因素2：屏幕中心距离（ScreenCenterDistance）—— 权重 30%
//     物体在屏幕上越偏离视口中心，人眼对其细节感知越弱
//     利用"注视点渲染"的思想：中心区域高精度，外围区域降精度
//     映射范围：0.0（视口正中心）→ LOD 0，1.0（视口边缘）→ LOD 4
// 
// 计算公式：
//   DistanceFactor = clamp(Distance / 10000, 0, 1) × 4.0
//   CenterFactor   = clamp(ScreenCenterDistance, 0, 1) × 4.0
//   FinalLOD       = round(DistanceFactor × 0.7 + CenterFactor × 0.3)
//   结果 clamp 到整数区间 [0, 4]
// =====================================================================================
int32 UProxyMeshGenerator::GetLODLevel(float Distance, float ScreenCenterDistance)
{
	// 因素1：摄像机距离 → 归一化到 [0, 1]，再映射到 [0, 4]
	// 距离 0       → DistanceFactor = 0（最高精度）
	// 距离 10000+  → DistanceFactor = 4（最低精度）
	float DistanceFactor = FMath::Clamp(Distance / 10000.0f, 0.0f, 1.0f) * 4.0f;

	// 因素2：屏幕中心距离 → 已归一化为 [0, 1]，直接映射到 [0, 4]
	// 屏幕正中心 0.0 → CenterFactor = 0（最高精度）
	// 屏幕边缘   1.0 → CenterFactor = 4（最低精度）
	float CenterFactor = FMath::Clamp(ScreenCenterDistance, 0.0f, 1.0f) * 4.0f;

	// 加权求和：摄像机距离占主导（70%），屏幕中心距离补充外围衰减（30%）
	float FinalLODFloat = (DistanceFactor * 0.7f) + (CenterFactor * 0.3f);

	// 四舍五入为整数，并限制在 [0, 4] 范围内
	int32 LODLevel = FMath::Clamp(FMath::RoundToInt(FinalLODFloat), 0, 4);

	return LODLevel;
}

// =====================================================================================
// GetScreenCenterDistance —— 计算屏幕中心距离
// 
// 将物体世界坐标投影到屏幕空间，计算到屏幕中心的距离
// 
// @param WorldLocation 物体世界坐标
// @param PlayerController 玩家控制器（用于投影）
// @return 到屏幕中心的归一化距离（0.0=中心，1.0=边缘或之外）
// =====================================================================================
float UProxyMeshGenerator::GetScreenCenterDistance(FVector WorldLocation, APlayerController* PlayerController)
{
	if (!PlayerController)
	{
		return 1.0f;  // 无控制器时返回最大值
	}

	FVector2D ScreenPosition;
	bool bIsOnScreen = PlayerController->ProjectWorldLocationToScreen(WorldLocation, ScreenPosition);

	if (!bIsOnScreen)
	{
		// 不可见（在屏幕外），返回最大值
		return 1.0f;
	}

	// 获取视口大小
	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	if (ViewportSizeX == 0 || ViewportSizeY == 0)
	{
		return 0.0f;
	}

	// 计算屏幕中心点
	FVector2D ScreenCenter(ViewportSizeX / 2.0f, ViewportSizeY / 2.0f);

	// 计算到屏幕中心的距离（使用 FVector2D 的 Distance 方法）
	float DistanceToCenter = FVector2D::Distance(ScreenPosition, ScreenCenter) / ScreenCenter.Size();

	// 限制在 [0, 1] 范围内
	return FMath::Clamp(DistanceToCenter, 0.0f, 1.0f);
}

// =====================================================================================
// GetCameraDistance —— 获取到摄像机的距离
// 
// @param WorldLocation 物体世界坐标
// @param CameraManager 摄像机管理器
// @return 到摄像机的欧氏距离（单位：厘米）
// =====================================================================================
float UProxyMeshGenerator::GetCameraDistance(FVector WorldLocation, APlayerCameraManager* CameraManager)
{
	if (!CameraManager)
	{
		// 如果没有摄像机管理器，返回0
		return 0.0f;
	}

	return FVector::Dist(WorldLocation, CameraManager->GetCameraLocation());
}

// =====================================================================================
// CalculateLODLevel —— 综合计算LOD等级（一次性获取所有参数）
// 
// 简化版：传入位置和控制器，自动计算两个因素并返回LOD等级
// 
// @param WorldLocation  物体世界坐标
// @param PlayerController 玩家控制器
// @return LOD等级索引（0~4）
// =====================================================================================
int32 UProxyMeshGenerator::CalculateLODLevel(FVector WorldLocation, APlayerController* PlayerController)
{
	if (!PlayerController)
	{
		return 0;  // 默认返回最高精度
	}

	// 获取摄像机管理器
	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	if (!CameraManager)
	{
		return 0;
	}

	// 计算摄像机距离
	float Distance = GetCameraDistance(WorldLocation, CameraManager);

	// 计算屏幕中心距离
	float ScreenCenterDistance = GetScreenCenterDistance(WorldLocation, PlayerController);

	// 计算LOD等级
	return GetLODLevel(Distance, ScreenCenterDistance);
}
