#include "VulkanCommon.h"
#include "VulkanDevice.h"
#include "VulkanFence.h"

USING_NAMESPACE(Spectre)

std::shared_ptr<VulkanFence> VulkanFence::CreateFence(const VulkanDevice& vulkanDevice)
{
	auto* pFence = new VulkanFence(vulkanDevice);
	return std::shared_ptr<VulkanFence>(pFence);
}

VulkanFence::~VulkanFence()
{
	Destory();
}

void VulkanFence::Destory()
{
	vkDestroyFence(m_Device.GetVkDevice(), m_VkFence, nullptr);
	m_VkFence = VK_NULL_HANDLE;
}

VulkanFence::VulkanFence(const VulkanDevice& vulkanDevice):
	m_Device(vulkanDevice)
{
	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	vkCreateFence(m_Device.GetVkDevice(), &fenceCreateInfo, nullptr, &m_VkFence);
}
