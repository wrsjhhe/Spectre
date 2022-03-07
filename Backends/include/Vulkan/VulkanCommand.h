#pragma once
#include <memory>
#include <vector>
#include <functional>
#include <vulkan.h>
BEGIN_NAMESPACE_SPECTRE

class VulkanCommandPool;

class VulkanCommand
{
public:
	static std::vector<std::shared_ptr<VulkanCommand>> Create(
		const VkCommandPool& commandPool, uint32_t size);

public:
	~VulkanCommand();
	VkCommandBuffer& GetVkCommandBuffer() { return m_VkCommandBuffer; }

	void RecordCommond(std::function<void(VkCommandBuffer)> recordCmd);

	void Submit(VulkanQueue& queue);

	void Free();
private:
	VulkanCommand(const VkCommandPool& commandPool, VkCommandBuffer buffer);
public:
	std::vector<VkSemaphore> SignalSemaphore;
	std::vector < VkPipelineStageFlags> WaitStageMask;
	std::vector<VkSemaphore> WaitSemaphore;
private:
	VkCommandPool								 m_CommandPool;
	VkCommandBuffer								 m_VkCommandBuffer;
	VkFence										 m_VkFence = VK_NULL_HANDLE;
	bool                                         m_IsEnd = false;
};



END_NAMESPACE_SPECTRE