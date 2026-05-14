// =====================================================================================
// PerformanceMonitorLibrary.cpp
// 
// 性能监控库实现 —— 用于论文实验数据采集
// 
// 注意：UE5.6中移除了直接的DrawCall API
// 实际DrawCall可以通过控制台命令 "stat rhi" 查看
// 或者在蓝图中使用 "Get GPU Benchmarks" 插件
// =====================================================================================

#include "PerformanceMonitorLibrary.h"
#include "Engine/Engine.h"

extern ENGINE_API float GAverageFPS;

// 获取当前FPS
float UPerformanceMonitorLibrary::GetCurrentFPS()
{
	return GAverageFPS;
}

// 获取Draw Call数量
// UE5.6移除了GNumDrawCallsRHI
// 返回0，实际值通过控制台 "stat rhi" 查看
int32 UPerformanceMonitorLibrary::GetDrawCallCount()
{
	// 可以在此处添加自定义统计逻辑
	// 当前简化返回0
	return 0;
}
