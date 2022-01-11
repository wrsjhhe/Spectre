#pragma once
#include <stdint.h>
#include <stdexcept>

#include "CommonDefinitions.h"
#include "Platform.h"


#if defined(NDEBUG)
#define VK_ENABLE_VALIDATION 0
#else
#define VK_ENABLE_VALIDATION 1
#endif

#if defined(SPE_PLATFORM_WINDOWS)
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#if defined(SPE_PLATFORM_ANDROID)
#define VK_USE_PLATFORM_ANDROID_KHR 1
#endif

#include "vulkan.h"

#define VK_CHECK(x,msg)                                             \
		if (x != VK_SUCCESS)                                        \
		{                                                           \
			throw std::runtime_error(msg);                          \
		}                                                           \

#define ASSERT_VK_HANDLE(handle,msg)            \
		if ((handle) == VK_NULL_HANDLE)		\
		{									\
			throw std::runtime_error(msg);	\
		}                                   \

#if !defined(NDEBUG) || defined(DEBUG) || defined(_DEBUG)
#	define VKB_DEBUG
#endif

BEGIN_NAMESPACE_RENDERSYSTEMS
/// <summary>
/// 格式是否只支持深度组件
/// </summary>
/// <param name="format">检测的格式</param>
/// <returns>True：只支持深度 False：并支持其他</returns>
bool IsDepthOnlyFormat(VkFormat format);


END_NAMESPACE_RENDERSYSTEMS
