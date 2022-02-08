#include "VulkanCommon.h"
#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "MathDef.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3.h>
USING_NAMESPACE(Spectre)


VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) 
{
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
		return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes) 
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


VulkanSwapChain::VulkanSwapChain(std::shared_ptr<const VulkanInstance> vulkanInstance,
	std::shared_ptr<const VulkanDevice> vulkanDevice,
	const NativeWindow& window, uint32_t width, uint32_t height):
	m_VulkanInstance(vulkanInstance),
	m_VulkanDevice(vulkanDevice),
	m_Window(window),
	m_Width(width),
	m_Height(height)
{
	VkDevice device = m_VulkanDevice->GetVkDevice();
	VkPhysicalDevice physicalDevice = m_VulkanDevice->GetPhysicalDevice().GetVkPhysicalDevice();
	CreateSurface();
	
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) 
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_VkSurface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicsFamily, (uint32_t)indices.presentFamily };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_VkSwapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(device, m_VkSwapChain, &imageCount, nullptr);
	m_VkImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, m_VkSwapChain, &imageCount, m_VkImages.data());

	m_VkSwapChainFormat = surfaceFormat.format;

	m_VkImageViews.resize(m_VkImages.size());
	m_ImageAcquiredSemaphore.resize(m_VkImages.size());
	for (uint32_t i = 0; i < m_VkImages.size(); i++) {
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_VkImages[i];
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = surfaceFormat.format;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		vkCreateImageView(device, &viewInfo, nullptr, &m_VkImageViews[i]);

		VkSemaphoreCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		vkCreateSemaphore(device, &createInfo, nullptr, &m_ImageAcquiredSemaphore[i]);
	}
}

VulkanSwapChain::~VulkanSwapChain()
{
	VkDevice device = m_VulkanDevice->GetVkDevice();

	for (uint32_t index = 0; index < m_ImageAcquiredSemaphore.size(); ++index) {
		vkDestroySemaphore(device, m_ImageAcquiredSemaphore[index], nullptr);
	}

	vkDestroySwapchainKHR(device, m_VkSwapChain, nullptr);
	vkDestroySurfaceKHR(m_VulkanInstance->GetVkInstance(), m_VkSurface, nullptr);
}

uint32_t VulkanSwapChain::AcquireImageIndex(VkSemaphore* outSemaphore)
{
	uint32_t imageIndex = 0;
	VkDevice device = m_VulkanDevice->GetVkDevice();
	const uint32_t prev = m_SemaphoreIndex;

	m_SemaphoreIndex = (m_SemaphoreIndex + 1) % m_ImageAcquiredSemaphore.size();
	VkResult result = vkAcquireNextImageKHR(device, m_VkSwapChain, MAX_uint64, m_ImageAcquiredSemaphore[m_SemaphoreIndex], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		m_SemaphoreIndex = prev;
		return (int32_t)SwapStatus::OutOfDate;
	}

	if (result == VK_ERROR_SURFACE_LOST_KHR) {
		m_SemaphoreIndex = prev;
		return (int32_t)SwapStatus::SurfaceLost;
	}

	*outSemaphore = m_ImageAcquiredSemaphore[m_SemaphoreIndex];
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

void VulkanSwapChain::DestorySwapChain()
{
	for (uint32_t i = 0; i < m_VkImageViews.size(); ++i) 
	{
		vkDestroyImageView(m_VulkanDevice->GetVkDevice(), m_VkImageViews[i], nullptr);
	}
}

void VulkanSwapChain::CreateSurface()
{
	if (m_VkSurface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(m_VulkanInstance->GetVkInstance(), m_VkSurface, NULL);
		m_VkSurface = VK_NULL_HANDLE;
	}

	//VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
	//surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	//surfaceCreateInfo.hinstance = GetModuleHandle(NULL);
	//surfaceCreateInfo.hwnd = (HWND)m_Window.hWnd;

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	auto window = glfwCreateWindow(1400, 900, "Vulkan", nullptr, nullptr);

	glfwSetWindowUserPointer(window, this); VK_PIPELINE_BIND_POINT_GRAPHICS;

	glfwSetWindowUserPointer(window, this);

	VkResult err = glfwCreateWindowSurface(m_VulkanInstance->GetVkInstance(), window, nullptr, &m_VkSurface);
	//VkResult err = vkCreateWin32SurfaceKHR(m_VulkanInstance->GetVkInstance(), &surfaceCreateInfo, nullptr, &m_VkSurface);
	VK_CHECK(err,"Failed create surface!")
}

VkExtent2D VulkanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		VkSurfaceCapabilitiesKHR surfProperties;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_VulkanDevice->GetPhysicalDevice().GetVkPhysicalDevice(), m_VkSurface, &surfProperties);


		VkExtent2D actualExtent = {
			static_cast<uint32_t>(m_Width),
			static_cast<uint32_t>(m_Height)
		};

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

VulkanSwapChain::SwapChainSupportDetails VulkanSwapChain::QuerySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_VkSurface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_VkSurface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_VkSurface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_VkSurface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_VkSurface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

VulkanSwapChain::QueueFamilyIndices VulkanSwapChain::FindQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_VkSurface, &presentSupport);

		if (queueFamily.queueCount > 0 && presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}