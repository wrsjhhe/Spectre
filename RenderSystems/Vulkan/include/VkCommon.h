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
/// ��ʽ�Ƿ�ֻ֧��������
/// </summary>
/// <param name="format">���ĸ�ʽ</param>
/// <returns>True��ֻ֧����� False����֧������</returns>
bool IsDepthOnlyFormat(VkFormat format);


SPECTRE_END_NAMESPACE //Spectre
SPECTRE_END_NAMESPACE //RenderSystem
