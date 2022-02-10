#include "VulkanCommon.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"

USING_NAMESPACE(Spectre)


std::shared_ptr<VulkanCommandPool> VulkanCommandPool::CreateCommandPool(const VulkanDevice& vulkanDevice)
{
	auto* commandPool = new VulkanCommandPool(vulkanDevice);
	return std::shared_ptr<VulkanCommandPool>(commandPool);
}

VulkanCommandPool::~VulkanCommandPool()
{
	Destroy();
}

VkCommandPool VulkanCommandPool::GetVkGraphicCommandPool() const
{
	return m_VkCommandPools.at(VK_QUEUE_GRAPHICS_BIT);
}

VkCommandPool VulkanCommandPool::GetVkComputeCommandPool() const
{
	return m_VkCommandPools.at(VK_QUEUE_COMPUTE_BIT);
}

VkCommandPool VulkanCommandPool::GetVkTransferCommandPool() const
{
	return m_VkCommandPools.at(VK_QUEUE_TRANSFER_BIT);
}

void VulkanCommandPool::Destroy()
{
	for (auto& kv : m_VkCommandPools)
	{
		if (kv.second != VK_NULL_HANDLE)
		{
			vkDestroyCommandPool(m_Device.GetVkDevice(), kv.second, nullptr);
			kv.second = VK_NULL_HANDLE;
		}
	}
	m_VkCommandPools.clear();
}

VulkanCommandPool::VulkanCommandPool(const VulkanDevice& vulkanDevice):
	m_Device(vulkanDevice)
{
	const VulkanQueue& graphicQueue = m_Device.GetGraphicQueue();
	const VulkanQueue& computeQueue = m_Device.GetComputeQueue();
	const VulkanQueue& transferQueue = m_Device.GetTransferQueue();

	VkCommandPool vkCommandPool;
	VkCommandPoolCreateInfo cmdPoolInfo{};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.queueFamilyIndex = graphicQueue.m_QueueFamilyIndex;
	cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	vkCreateCommandPool(m_Device.GetVkDevice(), &cmdPoolInfo, nullptr, &vkCommandPool);

	m_VkCommandPools[VK_QUEUE_GRAPHICS_BIT] = vkCommandPool;

	if (graphicQueue.m_QueueFamilyIndex != computeQueue.m_QueueFamilyIndex)
	{
		vkCommandPool = VK_NULL_HANDLE;
		cmdPoolInfo.queueFamilyIndex = computeQueue.m_QueueFamilyIndex;
		vkCreateCommandPool(m_Device.GetVkDevice(), &cmdPoolInfo, nullptr, &vkCommandPool);		
	}
	m_VkCommandPools[VK_QUEUE_COMPUTE_BIT] = vkCommandPool;

	if (graphicQueue.m_QueueFamilyIndex != transferQueue.m_QueueFamilyIndex)
	{
		vkCommandPool = VK_NULL_HANDLE;
		cmdPoolInfo.queueFamilyIndex = transferQueue.m_QueueFamilyIndex;
		vkCreateCommandPool(m_Device.GetVkDevice(), &cmdPoolInfo, nullptr, &vkCommandPool);
	}
	m_VkCommandPools[VK_QUEUE_TRANSFER_BIT] = vkCommandPool;
}
