// =====================================================================================
// ProxyMeshGenerator.h
// 
// 多因素LOD决策管理器
// 
// 综合两个因素决定物体的细节层级（LOD）：
//   因素1 —— 摄像机距离（Distance）：物体离摄像机越远，LOD等级越高
//   因素2 —— 屏幕中心距离（ScreenCenterDistance）：物体在屏幕上越偏离中心，
//            LOD等级越高，利用人眼对外围细节不敏感的特性
// 
// 该模块可结合UE Proxy Geometry Tool生成的代理网格使用
// =====================================================================================

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ProxyMeshGenerator.generated.h"

/**
 * UProxyMeshGenerator
 * 
 * 蓝图函数库，提供多因素LOD等级计算。
 * 所有函数均为static，可在任意蓝图或C++代码中直接调用。
 */
UCLASS(Blueprintable)
class PROXYMESHANDLOD_API UProxyMeshGenerator : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * 计算多因素LOD等级
	 * 
	 * 综合两个因素决定物体的细节层级（LOD）：
	 *   因素1 —— 摄像机距离（Distance）：物体离摄像机越远，LOD等级越高
	 *   因素2 —— 屏幕中心距离（ScreenCenterDistance）：物体在屏幕上越偏离中心
	 *            （即视觉外围区域），LOD等级越高
	 * 
	 * 计算公式：
	 *   DistanceFactor = clamp(Distance / 10000, 0, 1) * 4
	 *   CenterFactor = clamp(ScreenCenterDistance, 0, 1) * 4
	 *   FinalLOD = round(DistanceFactor * 0.7 + CenterFactor * 0.3)
	 *   结果 clamp 到 [0, 4]
	 * 
	 * @param Distance             物体到摄像机的世界空间距离（单位：厘米，UE默认单位）
	 * @param ScreenCenterDistance  物体投影到屏幕后距视口中心的归一化距离
	 *                            （0.0 = 屏幕正中心，1.0 = 视口边缘或不可见）
	 * @return LOD等级索引（0~4），0 = 最高精度，4 = 最低精度
	 */
	UFUNCTION(BlueprintCallable, Category="LOD|Management")
	static int32 GetLODLevel(float Distance, float ScreenCenterDistance);

	/**
	 * 计算屏幕中心距离
	 * 
	 * 将物体世界坐标投影到屏幕空间，计算到屏幕中心的距离
	 * 
	 * @param WorldLocation 物体世界坐标
	 * @param PlayerController 玩家控制器（用于投影）
	 * @return 到屏幕中心的归一化距离（0.0=中心，1.0=边缘或之外）
	 */
	UFUNCTION(BlueprintCallable, Category="LOD|Management")
	static float GetScreenCenterDistance(FVector WorldLocation, APlayerController* PlayerController);

	/**
	 * 获取到摄像机的距离
	 * 
	 * @param WorldLocation 物体世界坐标
	 * @param CameraManager 摄像机管理器
	 * @return 到摄像机的欧氏距离（单位：厘米）
	 */
	UFUNCTION(BlueprintCallable, Category="LOD|Management")
	static float GetCameraDistance(FVector WorldLocation, class APlayerCameraManager* CameraManager);

	/**
	 * 综合计算LOD等级（一次性获取所有参数）
	 * 
	 * 简化版：传入位置和控制器，自动计算两个因素并返回LOD等级
	 * 
	 * @param WorldLocation  物体世界坐标
	 * @param PlayerController 玩家控制器
	 * @return LOD等级索引（0~4）
	 */
	UFUNCTION(BlueprintCallable, Category="LOD|Management")
	static int32 CalculateLODLevel(FVector WorldLocation, APlayerController* PlayerController);
};
