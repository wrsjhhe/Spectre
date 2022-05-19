#include "VulkanSwapchain.h"

USING_NAMESPACE(Spectre)

static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}

VulkanSwapChain::VulkanSwapChain(VkSurfaceKHR surface, const SwapChainDesc& desc):
	m_Width(desc.Width),
	m_Height(desc.Height)
{
	auto* pEngine = VulkanEngine::GetInstance();
	VkPhysicalDevice physicalDevice = pEngine->GetVkPhysicalDevice();

	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

	//—°‘ÒPresentMode
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
	std::vector<VkPresentModeKHR> presentModes(presentModeCount);
	if (presentModeCount != 0) {
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());
	}

	VkPresentModeKHR presentMode = chooseSwapPresentMode(presentModes);

	m_ImageCount = g_ImageSize;
	//if (m_Context.m_VkSurfaceCapabilities.maxImageCount > 0 && m_ImageCount > m_Context.m_VkSurfaceCapabilities.maxImageCount)
	//{
	//	m_ImageCount = m_Context.m_VkSurfaceCapabilities.maxImageCount;
	//}
	//uint32_t support = false;

	//vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, pEngine->GetGraphicQueue().QueueFamilyIndex,m_Context.m_VkSurface,&support);

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = m_ImageCount;
	createInfo.imageFormat = g_SurfaceFormat.format;
	createInfo.imageColorSpace = g_SurfaceFormat.colorSpace;

	createInfo.imageExtent = { m_Width ,m_Height};
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

	createInfo.preTransform = surfaceCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	VK_CHECK(vkCreateSwapchainKHR(pEngine->GetVkDevice(), &createInfo, nullptr, &m_VkSwapChain), "Failed to create swap chain!");

	std::vector<VkImage> swapChainImages;
	swapChainImages.resize(m_ImageCount);
	vkGetSwapchainImagesKHR(pEngine->GetVkDevice(), m_VkSwapChain, &m_ImageCount, swapChainImages.data());

	m_SwapChainImageViews.resize(swapChainImages.size());
	for (size_t i = 0; i < swapChainImages.size(); i++) 
	{
		m_SwapChainImageViews[i] = VulkanImageView::Create(swapChainImages[i], g_SurfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	m_ImageAcquiredSemaphore.resize(m_ImageCount);
	for (uint32_t i = 0;i < m_ImageCount;++i)
	{
		m_ImageAcquiredSemaphore[i] = VulkanSemaphore::CreateSemaphore();
	}
}

VulkanSwapChain::~VulkanSwapChain()
{
	VkDevice device = VulkanEngine::GetInstance()->GetVkDevice();

	vkDestroySwapchainKHR(device, m_VkSwapChain, nullptr);
}

uint32_t VulkanSwapChain::AcquireImageIndex(std::shared_ptr<VulkanSemaphore>& outSemaphore)
{
	uint32_t imageIndex = 0;
	VkDevice device = VulkanEngine::GetInstance()->GetVkDevice();
	const uint32_t prev = m_SemaphoreIndex;

	m_SemaphoreIndex = (m_SemaphoreIndex + 1) % m_ImageAcquiredSemaphore.size();
	VkResult result = vkAcquireNextImageKHR(device, m_VkSwapChain, 0xffffffff, m_ImageAcquiredSemaphore[m_SemaphoreIndex]->GetVkSemaphore(), VK_NULL_HANDLE, &imageIndex);

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
