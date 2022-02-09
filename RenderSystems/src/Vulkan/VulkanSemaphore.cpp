#include "VulkanCommon.h"
#include "VulkanDevice.h"
#include "VulkanSemaphore.h"

USING_NAMESPACE(Spectre)

std::shared_ptr<VulkanSemaphore> VulkanSemaphore::CreateSemaphore(const VulkanDevice& vulkanDevice)
{
	auto* pSemaphore = new VulkanSemaphore(vulkanDevice);
	return std::shared_ptr<VulkanSemaphore>(pSemaphore);
}

VulkanSemaphore::~VulkanSemaphore()
{
	Destory();
}

void VulkanSemaphore::Destory()
{
	vkDestroySemaphore(m_Device.GetVkDevice(), m_VkSemaphore, nullptr);
}

VulkanSemaphore::VulkanSemaphore(const VulkanDevice& vulkanDevice):
	m_Device(vulkanDevice)
{
	VkSemaphoreCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	vkCreateSemaphore(m_Device.GetVkDevice(), &createInfo, nullptr, &m_VkSemaphore);
}

