#pragma once

#include <memory>
#include <vector>
BEGIN_NAMESPACE_SPECTRE

class VulkanCommandPool;

class VulkanCommandBuffers
{
public:
	static std::shared_ptr<VulkanCommandBuffers> CreataCommandBuffers(const std::shared_ptr<const VulkanDevice>& vulkanDevice,
		const std::shared_ptr<const VulkanCommandPool>& commandPool, uint32_t size);


public:
	~VulkanCommandBuffers();
	std::vector<VkCommandBuffer>& GetVkCommandBuffers() { return m_VkCommandBuffer; }

	void Free();
private:
	VulkanCommandBuffers(const std::shared_ptr<const VulkanDevice>& vulkanDevice, const std::shared_ptr<const VulkanCommandPool>& commandPool, uint32_t size);
private:
	std::shared_ptr<const VulkanDevice>			 m_DevicePtr;
	std::shared_ptr<const VulkanCommandPool>	 m_CommandPool;
	std::vector<VkCommandBuffer>                 m_VkCommandBuffer;
};

END_NAMESPACE_SPECTRE