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

VulkanContext::VulkanContext(VkInstance instance, VkPhysicalDevice physicalDevice, const VulkanDevice& device):
	m_VkInstance(instance),
	m_VkPhysicalDevice(physicalDevice),
	m_Device(device)
{

}

VulkanContext::~VulkanContext()
{
	vkDestroySurfaceKHR(m_VkInstance, m_VkSurface, nullptr);

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

void VulkanContext::Init()
{
	InitCommandPool();
	InitSwapchainParamaters();
}

void VulkanContext::CalculateSwapChainExtent(uint32_t& width, uint32_t& height)
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
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_VkPhysicalDevice, m_VkSurface, &surfProperties);

		width = std::max(m_VkSurfaceCapabilities.minImageExtent.width, std::min(m_VkSurfaceCapabilities.maxImageExtent.width, width));
		height = std::max(m_VkSurfaceCapabilities.minImageExtent.height, std::min(m_VkSurfaceCapabilities.maxImageExtent.height, height));
	}
}

void VulkanContext::InitSwapchainParamaters()
{
	//—°‘ÒSurfaceFormat
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_VkPhysicalDevice, m_VkSurface, &formatCount, nullptr);
	std::vector<VkSurfaceFormatKHR> formats(formatCount);
	if (formatCount != 0) {
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_VkPhysicalDevice, m_VkSurface, &formatCount, formats.data());
	}
	m_VkSurfaceFormat = chooseSwapSurfaceFormat(formats);
	m_VkSwapChainFormat = m_VkSurfaceFormat.format;
	//—°‘ÒPresentMode
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_VkPhysicalDevice, m_VkSurface, &presentModeCount, nullptr);
	std::vector<VkPresentModeKHR> presentModes(presentModeCount);
	if (presentModeCount != 0) {
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_VkPhysicalDevice, m_VkSurface, &presentModeCount, presentModes.data());
	}

	m_VkPresentMode = chooseSwapPresentMode(presentModes);

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_VkPhysicalDevice, m_VkSurface, &m_VkSurfaceCapabilities);
}

void VulkanContext::InitCommandPool()
{
	const VulkanQueue& graphicQueue = m_Device.GetGraphicQueue();
	const VulkanQueue& computeQueue = m_Device.GetComputeQueue();
	const VulkanQueue& transferQueue = m_Device.GetTransferQueue();

	VkCommandPool vkCommandPool;
	VkCommandPoolCreateInfo cmdPoolInfo{};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.queueFamilyIndex = graphicQueue.m_QueueFamilyIndex;
	cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	VK_CHECK(vkCreateCommandPool(m_Device.GetVkDevice(), &cmdPoolInfo, nullptr, &vkCommandPool), "Failed created VkCommandPool!");

	m_VkCommandPools[CommandPool_Type_Graphics] = vkCommandPool;

	if (graphicQueue.m_QueueFamilyIndex != computeQueue.m_QueueFamilyIndex)
	{
		vkCommandPool = VK_NULL_HANDLE;
		cmdPoolInfo.queueFamilyIndex = computeQueue.m_QueueFamilyIndex;
		VK_CHECK(vkCreateCommandPool(m_Device.GetVkDevice(), &cmdPoolInfo, nullptr, &vkCommandPool), "Failed created VkCommandPool!");
		m_VkCommandPools[CommandPool_Type_Compute] = vkCommandPool;
	}


	if (graphicQueue.m_QueueFamilyIndex != transferQueue.m_QueueFamilyIndex)
	{
		vkCommandPool = VK_NULL_HANDLE;
		cmdPoolInfo.queueFamilyIndex = transferQueue.m_QueueFamilyIndex;
		VK_CHECK(vkCreateCommandPool(m_Device.GetVkDevice(), &cmdPoolInfo, nullptr, &vkCommandPool), "Failed created VkCommandPool!");
		m_VkCommandPools[CommandPool_Type_Transfer] = vkCommandPool;
	}

}


VkCommandPool VulkanContext::GetVkGraphicCommandPool() const
{
	return m_VkCommandPools.at(CommandPool_Type_Graphics);
}

VkCommandPool VulkanContext::GetVkComputeCommandPool() const
{
	if (m_VkCommandPools.find(CommandPool_Type_Compute) != m_VkCommandPools.end())
	{
		return m_VkCommandPools.at(CommandPool_Type_Compute);
	}
	return m_VkCommandPools.at(CommandPool_Type_Graphics);
}

VkCommandPool VulkanContext::GetVkTransferCommandPool() const
{
	if (m_VkCommandPools.find(CommandPool_Type_Transfer) != m_VkCommandPools.end())
	{
		return m_VkCommandPools.at(CommandPool_Type_Transfer);
	}
	return m_VkCommandPools.at(CommandPool_Type_Graphics);
}