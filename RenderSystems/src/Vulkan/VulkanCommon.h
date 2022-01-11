#pragma once

#include "CommonDefinitions.h"
#include <stdint.h>
#include "vulkan.h"
#include "CommonDefinitions.h"
#if defined(NDEBUG)
#define VK_ENABLE_VALIDATION 0
#else
#define VK_ENABLE_VALIDATION 1
#endif

BEGIN_NAMESPACE_RENDERSYSTEMS
/// <summary>
/// 格式是否只支持深度组件
/// </summary>
/// <param name="format">检测的格式</param>
/// <returns>True：只支持深度 False：并支持其他</returns>
bool IsDepthOnlyFormat(VkFormat format);


END_NAMESPACE_RENDERSYSTEMS
