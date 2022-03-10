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


static uint32_t VertexAttributeToSize(VertexAttribute attribute)
{
    // count * sizeof(float)
    if (attribute == VertexAttribute::VertexAttribute_Position)
    {
        return 3 * sizeof(float);
    }
    else if (attribute == VertexAttribute::VertexAttribute_UV0)
    {
        return 2 * sizeof(float);
    }
    else if (attribute == VertexAttribute::VertexAttribute_UV1)
    {
        return 2 * sizeof(float);
    }
    else if (attribute == VertexAttribute::VertexAttribute_Normal)
    {
        return 3 * sizeof(float);
    }
    else if (attribute == VertexAttribute::VertexAttribute_Tangent)
    {
        return 4 * sizeof(float);
    }
    else if (attribute == VertexAttribute::VertexAttribute_Color)
    {
        return 4 * sizeof(float);
    }
    else if (attribute == VertexAttribute::VertexAttribute_SkinWeight)
    {
        return 4 * sizeof(float);
    }
    else if (attribute == VertexAttribute::VertexAttribute_SkinIndex)
    {
        return 4 * sizeof(float);
    }
    else if (attribute == VertexAttribute::VertexAttribute_SkinPack)
    {
        return 3 * sizeof(float);
    }
    else if (attribute == VertexAttribute::VertexAttribute_Custom0 ||
        attribute == VertexAttribute::VertexAttribute_Custom1 ||
        attribute == VertexAttribute::VertexAttribute_Custom2 ||
        attribute == VertexAttribute::VertexAttribute_Custom3
        )
    {
        return 4 * sizeof(float);
    }
    else if (attribute == VertexAttribute::VertexAttribute_InstanceFloat1)
    {
        return 1 * sizeof(float);
    }
    else if (attribute == VertexAttribute::VertexAttribute_InstanceFloat2)
    {
        return 2 * sizeof(float);
    }
    else if (attribute == VertexAttribute::VertexAttribute_InstanceFloat3)
    {
        return 3 * sizeof(float);
    }
    else if (attribute == VertexAttribute::VertexAttribute_InstanceFloat4)
    {
        return 4 * sizeof(float);
    }

    return 0;
}

static VkFormat VertexAttributeToVkFormat(VertexAttribute attribute)
{
    VkFormat format = VK_FORMAT_R32G32B32_SFLOAT;
    if (attribute == VertexAttribute::VertexAttribute_Position)
    {
        format = VK_FORMAT_R32G32B32_SFLOAT;
    }
    else if (attribute == VertexAttribute::VertexAttribute_UV0)
    {
        format = VK_FORMAT_R32G32_SFLOAT;
    }
    else if (attribute == VertexAttribute::VertexAttribute_UV1)
    {
        format = VK_FORMAT_R32G32_SFLOAT;
    }
    else if (attribute == VertexAttribute::VertexAttribute_Normal)
    {
        format = VK_FORMAT_R32G32B32_SFLOAT;
    }
    else if (attribute == VertexAttribute::VertexAttribute_Tangent)
    {
        format = VK_FORMAT_R32G32B32A32_SFLOAT;
    }
    else if (attribute == VertexAttribute::VertexAttribute_Color)
    {
        format = VK_FORMAT_R32G32B32_SFLOAT;
    }
    else if (attribute == VertexAttribute::VertexAttribute_SkinPack)
    {
        format = VK_FORMAT_R32G32B32_SFLOAT;
    }
    else if (attribute == VertexAttribute::VertexAttribute_SkinWeight)
    {
        format = VK_FORMAT_R32G32B32A32_SFLOAT;
    }
    else if (attribute == VertexAttribute::VertexAttribute_SkinIndex)
    {
        format = VK_FORMAT_R32G32B32A32_SFLOAT;
    }
    else if (attribute == VertexAttribute::VertexAttribute_Custom0 ||
        attribute == VertexAttribute::VertexAttribute_Custom1 ||
        attribute == VertexAttribute::VertexAttribute_Custom2 ||
        attribute == VertexAttribute::VertexAttribute_Custom3
        )
    {
        format = VK_FORMAT_R32G32B32A32_SFLOAT;
    }
    else if (attribute == VertexAttribute::VertexAttribute_InstanceFloat1)
    {
        format = VK_FORMAT_R32_SFLOAT;
    }
    else if (attribute == VertexAttribute::VertexAttribute_InstanceFloat2)
    {
        format = VK_FORMAT_R32G32_SFLOAT;
    }
    else if (attribute == VertexAttribute::VertexAttribute_InstanceFloat3)
    {
        format = VK_FORMAT_R32G32B32_SFLOAT;
    }
    else if (attribute == VertexAttribute::VertexAttribute_InstanceFloat4)
    {
        format = VK_FORMAT_R32G32B32A32_SFLOAT;
    }

    return format;
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


void VulkanContext::SetVertexDesc(const std::vector<VertexAttribute>& attrs)
{
	uint32_t stride = 0;
	for (uint32_t i = 0; i < attrs.size(); ++i) {
		stride += VertexAttributeToSize(attrs[i]);
	}

	m_InputBindingDesc.resize(1);
	m_InputBindingDesc[0].binding = 0;
	m_InputBindingDesc[0].stride = stride;
	m_InputBindingDesc[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	//create VertexInputAttributeDescription
	m_InputAttributeDesc.resize(attrs.size());
	uint32_t offset = 0;
	for (uint32_t i = 0; i < attrs.size(); ++i)
	{
		m_InputAttributeDesc[i].binding = 0;
		m_InputAttributeDesc[i].location = i;
		m_InputAttributeDesc[i].format = VertexAttributeToVkFormat(attrs[i]);
		m_InputAttributeDesc[i].offset = offset;
		offset += VertexAttributeToSize(attrs[i]);
	}
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
void VulkanContext::InitCommandPool()
{
    auto vkDevice = m_VulkanEnginePtr->GetVkDevice();
	const auto& graphicQueue = m_VulkanEnginePtr->GetGraphicQueue();
	const auto& computeQueue = m_VulkanEnginePtr->GetComputeQueue();
	const auto& transferQueue = m_VulkanEnginePtr->GetTransferQueue();

	VkCommandPool vkCommandPool;
	VkCommandPoolCreateInfo cmdPoolInfo{};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.queueFamilyIndex = graphicQueue.QueueFamilyIndex;
	cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	VK_CHECK(vkCreateCommandPool(vkDevice, &cmdPoolInfo, nullptr, &vkCommandPool), "Failed created VkCommandPool!");

	m_VkCommandPools[CommandPool_Type_Graphics] = vkCommandPool;

	if (graphicQueue.QueueFamilyIndex != computeQueue.QueueFamilyIndex)
	{
		vkCommandPool = VK_NULL_HANDLE;
		cmdPoolInfo.queueFamilyIndex = computeQueue.QueueFamilyIndex;
		VK_CHECK(vkCreateCommandPool(vkDevice, &cmdPoolInfo, nullptr, &vkCommandPool), "Failed created VkCommandPool!");
		m_VkCommandPools[CommandPool_Type_Compute] = vkCommandPool;
	}


	if (graphicQueue.QueueFamilyIndex != transferQueue.QueueFamilyIndex)
	{
		vkCommandPool = VK_NULL_HANDLE;
		cmdPoolInfo.queueFamilyIndex = transferQueue.QueueFamilyIndex;
		VK_CHECK(vkCreateCommandPool(vkDevice, &cmdPoolInfo, nullptr, &vkCommandPool), "Failed created VkCommandPool!");
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
