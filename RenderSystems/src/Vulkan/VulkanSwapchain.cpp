#include "VulkanCommon.h"
#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanSemaphore.h"
#include "VulkanSwapchain.h"
#include "MathDef.h"
USING_NAMESPACE(Spectre)

VulkanSwapChain::VulkanSwapChain(const VulkanDevice& device, const VulkanContext& context, const SwapChainDesc& desc):
	m_Device(device),
	m_Context(context),
	m_Width(desc.Width),
	m_Height(desc.Height)
{
	VkPhysicalDevice physicalDevice = m_Device.GetPhysicalDevice().GetVkPhysicalDevice();

	m_ImageCount = m_Context.m_VkSurfaceCapabilities.minImageCount + 1;
	if (m_Context.m_VkSurfaceCapabilities.maxImageCount > 0 && m_ImageCount > m_Context.m_VkSurfaceCapabilities.maxImageCount)
	{
		m_ImageCount = m_Context.m_VkSurfaceCapabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_Context.m_VkSurface;

	createInfo.minImageCount = m_ImageCount;
	createInfo.imageFormat = m_Context.m_VkSurfaceFormat.format;
	createInfo.imageColorSpace = m_Context.m_VkSurfaceFormat.colorSpace;

	createInfo.imageExtent = { m_Width ,m_Height};
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

	createInfo.preTransform = m_Context.m_VkSurfaceCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = m_Context.m_VkPresentMode;
	createInfo.clipped = VK_TRUE;

	VK_CHECK(vkCreateSwapchainKHR(m_Device.GetVkDevice(), &createInfo, nullptr, &m_VkSwapChain), "Failed to create swap chain!");

	m_Images = VulkanImages::CreateSwapChainImage(m_Device, *this);
	m_ImageAcquiredSemaphore.resize(m_ImageCount);
	for (uint32_t i = 0;i < m_ImageCount;++i)
	{
		m_ImageAcquiredSemaphore[i] = VulkanSemaphore::CreateSemaphore(m_Device);
	}
}

VulkanSwapChain::~VulkanSwapChain()
{
	m_Images = nullptr;
	VkDevice device = m_Device.GetVkDevice();

	vkDestroySwapchainKHR(device, m_VkSwapChain, nullptr);
}

uint32_t VulkanSwapChain::AcquireImageIndex(std::shared_ptr<VulkanSemaphore>& outSemaphore)
{
	uint32_t imageIndex = 0;
	VkDevice device = m_Device.GetVkDevice();
	const uint32_t prev = m_SemaphoreIndex;

	m_SemaphoreIndex = (m_SemaphoreIndex + 1) % m_ImageAcquiredSemaphore.size();
	VkResult result = vkAcquireNextImageKHR(device, m_VkSwapChain, MAX_uint64, m_ImageAcquiredSemaphore[m_SemaphoreIndex]->GetVkSemaphore(), VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		m_SemaphoreIndex = prev;
		return (int32_t)SwapStatus::OutOfDate;
	}

	if (result == VK_ERROR_SURFACE_LOST_KHR) {
		m_SemaphoreIndex = prev;
		return (int32_t)SwapStatus::SurfaceLost;
	}

	outSemaphore = m_ImageAcquiredSemaphore[m_SemaphoreIndex];
	m_CurrentImageIndex = (uint32_t)imageIndex;

	return m_CurrentImageIndex;
}

VulkanSwapChain::SwapStatus VulkanSwapChain::Present(VkQueue presentQueue, VkSemaphore* doneSemaphore)
{
	if (m_CurrentImageIndex == -1) {
		return SwapStatus::Healthy;
	}

	VkPresentInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	createInfo.waitSemaphoreCount = doneSemaphore == nullptr ? 0 : 1;
	createInfo.pWaitSemaphores = doneSemaphore;
	createInfo.swapchainCount = 1;
	createInfo.pSwapchains = &m_VkSwapChain;
	createInfo.pImageIndices = (uint32_t*)&m_CurrentImageIndex;

	VkResult presentResult = vkQueuePresentKHR(presentQueue, &createInfo);

	if (presentResult == VK_ERROR_OUT_OF_DATE_KHR) {
		return SwapStatus::OutOfDate;
	}

	if (presentResult == VK_ERROR_SURFACE_LOST_KHR) {
		return SwapStatus::SurfaceLost;
	}

	if (presentResult != VK_SUCCESS && presentResult != VK_SUBOPTIMAL_KHR) {
		abort();
	}

	return SwapStatus::Healthy;
}
