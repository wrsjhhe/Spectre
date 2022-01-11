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
/// ��ʽ�Ƿ�ֻ֧��������
/// </summary>
/// <param name="format">���ĸ�ʽ</param>
/// <returns>True��ֻ֧����� False����֧������</returns>
bool IsDepthOnlyFormat(VkFormat format);


END_NAMESPACE_RENDERSYSTEMS
