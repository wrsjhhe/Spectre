#pragma once
#include <stdint.h>
#include <stdexcept>
#include < stdlib.h >
#include "CommonDefinitions.h"
#include "Platform.h"
#include "LogManager.h"
#if !defined(NDEBUG) || defined(DEBUG) || defined(_DEBUG)
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
const uint32_t  g_VkVersion = VK_API_VERSION_1_2;


#define VK_CHECK(x,msg)												     \
	do																	 \
	{																	 \
		if (x)															 \
		{																 \
			Common::LogManager::GetInstance()->Error(msg);				 \
			abort();													 \
		}																 \
	} while (0);														 \



constexpr VkPipelineStageFlags VK_PIPELINE_STAGE_ALL_SHADERS =
VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT |
VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT |
VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT |
VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR |
VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV |
VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV;

constexpr VkPipelineStageFlags VK_PIPELINE_STAGE_ALL_TRANSFER =
VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT |
VK_PIPELINE_STAGE_TRANSFER_BIT |
VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT |
VK_PIPELINE_STAGE_HOST_BIT |
VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
