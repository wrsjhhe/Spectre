#include "VulkanCommon.h"
#include "VulkanContext.h"

USING_NAMESPACE(Spectre)

static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
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


//*************************************************************************************************************************************//

VulkanContext::VulkanContext(VulkanEngine* pEngine):
    m_VulkanEnginePtr(pEngine)
{

}

VulkanContext::~VulkanContext()
{
	vkDestroySurfaceKHR(m_VulkanEnginePtr->GetVkInstance(), m_VkSurface, nullptr);

	for (auto& kv : m_VkCommandPools)
	{
		if (kv.second != VK_NULL_HANDLE)
		{
			vkDestroyCommandPool(m_VulkanEnginePtr->GetVkDevice(), kv.second, nullptr);
			kv.second = VK_NULL_HANDLE;
		}
	}
	m_VkCommandPools.clear();
}


void VulkanContext::CalcSwapChainExtent(uint32_t& width, uint32_t& height)
{
	if (m_VkSurfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
	{
		width = m_VkSurfaceCapabilities.currentExtent.width;
		height = m_VkSurfaceCapabilities.currentExtent.height;
		return ;
	}
	else 
	{
		VkSurfaceCapabilitiesKHR surfProperties;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_VulkanEnginePtr->GetVkPhysicalDevice(), m_VkSurface, &surfProperties);

		width = std::max(m_VkSurfaceCapabilities.minImageExtent.width, std::min(m_VkSurfaceCapabilities.maxImageExtent.width, width));
		height = std::max(m_VkSurfaceCapabilities.minImageExtent.height, std::min(m_VkSurfaceCapabilities.maxImageExtent.height, height));
	}
}

void VulkanContext::CalcSwapchainParamaters(VkSurfaceKHR surface)
{
    if (m_VkSurface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(m_VulkanEnginePtr->GetVkInstance(), m_VkSurface, NULL);
        m_VkSurface = VK_NULL_HANDLE;
    }
    m_VkSurface = surface;
	ReCalcSwapchainParamaters();
}
void VulkanContext::ReCalcSwapchainParamaters()
{
    auto vkPhysicalDevice = m_VulkanEnginePtr->GetVkPhysicalDevice();
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, m_VkSurface, &formatCount, nullptr);
	std::vector<VkSurfaceFormatKHR> formats(formatCount);
	if (formatCount != 0) {
		vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, m_VkSurface, &formatCount, formats.data());
	}
	m_VkSurfaceFormat = chooseSwapSurfaceFormat(formats);
	m_VkSwapChainFormat = m_VkSurfaceFormat.format;
	//—°‘ÒPresentMode
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, m_VkSurface, &presentModeCount, nullptr);
	std::vector<VkPresentModeKHR> presentModes(presentModeCount);
	if (presentModeCount != 0) {
		vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, m_VkSurface, &presentModeCount, presentModes.data());
	}

	m_VkPresentMode = chooseSwapPresentMode(presentModes);

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice, m_VkSurface, &m_VkSurfaceCapabilities);
}
