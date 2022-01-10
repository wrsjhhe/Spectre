#pragma once
#include <memory>
#include "vulkan.h"

class VulkanInstace : public std::enable_shared_from_this<VkInstance>
{
public:

private:
	VkInstance    m_VkInstance = VK_NULL_HANDLE;
};