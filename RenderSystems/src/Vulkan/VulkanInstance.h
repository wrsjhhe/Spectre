#pragma once
#include <memory>
#include "vulkan.h"
#include "VulkanCommon.h"

BEGIN_NAMESPACE_RENDERSYSTEMS

class VulkanInstace : public std::enable_shared_from_this<VkInstance>
{
public:

private:
	VkInstance    m_VkInstance = VK_NULL_HANDLE;
};

END_NAMESPACE_RENDERSYSTEMS