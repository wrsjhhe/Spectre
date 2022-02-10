#pragma once

#include <memory>
#include <vector>
#include <vulkan.h>
BEGIN_NAMESPACE_SPECTRE

class VulkanCommandPool;

class VulkanCommandBuffers: public Noncopyable
{
public:
	static std::shared_ptr<VulkanCommandBuffers> CreataGraphicBuffers(const VulkanDevice& vulkanDevice,
		const VulkanCommandPool& commandPool, uint32_t size);
	static std::shared_ptr<VulkanCommandBuffers> CreataTransferBuffers(const VulkanDevice& vulkanDevice,
		const VulkanCommandPool& commandPool, uint32_t size);

public:
	~VulkanCommandBuffers();
	std::vector<VkCommandBuffer>& GetVkCommandBuffers() { return m_VkCommandBuffer; }

	void Free();
private:
	VulkanCommandBuffers(const VulkanDevice& vulkanDevice, const VkCommandPool& commandPool, uint32_t size);
private:
	const VulkanDevice&							 m_Device;
	VkCommandPool								 m_CommandPool;
	std::vector<VkCommandBuffer>                 m_VkCommandBuffer;
};

END_NAMESPACE_SPECTRE