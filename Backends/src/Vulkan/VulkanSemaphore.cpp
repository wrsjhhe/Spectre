#include "VulkanCommon.h"
#include "VulkanEngine.h"
#include "VulkanSemaphore.h"

USING_NAMESPACE(Spectre)

std::shared_ptr<VulkanSemaphore> VulkanSemaphore::CreateSemaphore()
{
	auto* pSemaphore = new VulkanSemaphore();
	return std::shared_ptr<VulkanSemaphore>(pSemaphore);
}

VulkanSemaphore::~VulkanSemaphore()
{
	Destory();
}

void VulkanSemaphore::Destory()
{
	vkDestroySemaphore(VulkanEngine::GetInstance()->GetVkDevice(), m_VkSemaphore, nullptr);
}

VulkanSemaphore::VulkanSemaphore()
{
	VkSemaphoreCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	vkCreateSemaphore(VulkanEngine::GetInstance()->GetVkDevice(), &createInfo, nullptr, &m_VkSemaphore);
}

