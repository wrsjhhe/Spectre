#include "VulkanCommon.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"

USING_NAMESPACE(Spectre)


std::shared_ptr<VulkanCommandPool> VulkanCommandPool::CreateCommandPool(const std::shared_ptr<const VulkanDevice>& vulkanDevice)
{
	auto* commandPool = new VulkanCommandPool(vulkanDevice);
	return std::shared_ptr<VulkanCommandPool>(commandPool);
}

VulkanCommandPool::~VulkanCommandPool()
{
	Destroy();
}

void VulkanCommandPool::Destroy()
{
	vkDestroyCommandPool(m_DevicePtr->GetVkDevice(), m_VkCommandPool, nullptr);
}

VulkanCommandPool::VulkanCommandPool(const std::shared_ptr<const VulkanDevice>& vulkanDevice):
	m_DevicePtr(vulkanDevice)
{
	VkCommandPoolCreateInfo cmdPoolInfo{};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.queueFamilyIndex = m_DevicePtr->GetPhysicalDevice().FindQueueFamily(VK_QUEUE_GRAPHICS_BIT);
	cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	vkCreateCommandPool(m_DevicePtr->GetVkDevice(), &cmdPoolInfo, nullptr, &m_VkCommandPool);
}
