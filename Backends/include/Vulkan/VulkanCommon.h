#pragma once
#include <stdint.h>
#include <stdexcept>
#include < stdlib.h >
#include "SpectreDef.h"
#include "Platform.h"
#include "LogManager.h"
#if defined(SPE_DEBUG)
#	define VKB_DEBUG
#endif

#if defined(SPE_PLATFORM_WINDOWS)
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#if defined(SPE_PLATFORM_ANDROID)
#define VK_USE_PLATFORM_ANDROID_KHR 1
#endif

#define VK_ENABLE_BETA_EXTENSIONS
#include "vulkan.h"
#include "vulkan_beta.h"

#define VK_CHECK(x,msg)	EXP_CHECK(x == VK_SUCCESS,msg)

BEGIN_NAMESPACE_SPECTRE

const uint32_t  g_VkVersion = VK_API_VERSION_1_0;
const uint32_t  g_ImageSize = 3;

END_NAMESPACE_SPECTRE

