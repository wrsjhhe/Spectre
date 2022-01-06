#pragma once

#include "CommonDefinitions.h"
#include <stdint.h>
#include "vulkan.h"

#if defined(NDEBUG)
#define VK_ENABLE_VALIDATION 0
#else
#define VK_ENABLE_VALIDATION 1
#endif

SPECTRE_BEGIN_NAMESPACE(Spectre)
SPECTRE_BEGIN_NAMESPACE(RenderSystem)

/// <summary>
/// 格式是否只支持深度组件
/// </summary>
/// <param name="format">检测的格式</param>
/// <returns>True：只支持深度 False：并支持其他</returns>
bool IsDepthOnlyFormat(VkFormat format);


SPECTRE_END_NAMESPACE //Spectre
SPECTRE_END_NAMESPACE //RenderSystem
