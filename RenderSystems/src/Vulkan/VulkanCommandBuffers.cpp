#include "VulkanCommon.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffers.h"

USING_NAMESPACE(Spectre)

std::shared_ptr<VulkanCommandBuffers> VulkanCommandBuffers::CreataCommandBuffers(const std::shared_ptr<const VulkanDevice>& vulkanDevice,
	const std::shared_ptr<const VulkanCommandPool>& commandPool, uint32_t size)
{
	auto* commandBuffers = new VulkanCommandBuffers(vulkanDevice, commandPool, size);
	return std::shared_ptr<VulkanCommandBuffers>(commandBuffers);
}

VulkanCommandBuffers::~VulkanCommandBuffers()
{
	Free();
}

VulkanCommandBuffers::VulkanCommandBuffers(const std::shared_ptr<const VulkanDevice>& vulkanDevice,
	const std::shared_ptr<const VulkanCommandPool>& commandPool, uint32_t size) :
	m_DevicePtr(vulkanDevice),
	m_CommandPool(commandPool)
{
	m_VkCommandBuffer.resize(size);
	VkCommandBufferAllocateInfo cmdBufferInfo{};
	cmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdBufferInfo.commandBufferCount = size;
	cmdBufferInfo.commandPool = m_CommandPool->GetVkCommandPool();

	vkAllocateCommandBuffers(m_DevicePtr->GetVkDevice(), &cmdBufferInfo, m_VkCommandBuffer.data());
}

void VulkanCommandBuffers::Free()
{
	vkFreeCommandBuffers(m_DevicePtr->GetVkDevice(), m_CommandPool->GetVkCommandPool(), m_VkCommandBuffer.size(), m_VkCommandBuffer.data());
}
