#pragma once

BEGIN_NAMESPACE_SPECTRE

class VulkanQueue
{
public:
	VulkanQueue();
	explicit VulkanQueue(const VkDevice& device, uint32_t familyIndex);
	VulkanQueue(const VulkanQueue& other);


	int32_t   m_QueueFamilyIndex = -1;
	VkQueue    m_VkQueue = VK_NULL_HANDLE;
};

END_NAMESPACE_SPECTRE