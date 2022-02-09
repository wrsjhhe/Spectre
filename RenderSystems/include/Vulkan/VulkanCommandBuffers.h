#pragma once

#include <memory>
#include <vector>
BEGIN_NAMESPACE_SPECTRE

class VulkanCommandPool;

class VulkanCommandBuffers
{
public:
	static std::shared_ptr<VulkanCommandBuffers> CreataCommandBuffers(const VulkanDevice& vulkanDevice,
		const VulkanCommandPool& commandPool, uint32_t size);


public:
	~VulkanCommandBuffers();
	std::vector<VkCommandBuffer>& GetVkCommandBuffers() { return m_VkCommandBuffer; }

	void Free();
private:
	VulkanCommandBuffers(const VulkanDevice& vulkanDevice, const VulkanCommandPool& commandPool, uint32_t size);
private:
	const VulkanDevice&							 m_Device;
	const VulkanCommandPool&					 m_CommandPool;
	std::vector<VkCommandBuffer>                 m_VkCommandBuffer;
};

END_NAMESPACE_SPECTRE