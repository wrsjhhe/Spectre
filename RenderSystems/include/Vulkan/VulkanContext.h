#pragma once
#include <map>
#include "VulkanGraphicTypes.h"
#include "VulkanDevice.h"
BEGIN_NAMESPACE_SPECTRE

class VulkanContext
{
public:
	VulkanContext(VkInstance instance, VkPhysicalDevice physicalDevice,const VulkanDevice& device);
	~VulkanContext();
	void Init();

	void CalculateSwapChainExtent(uint32_t& width, uint32_t& height);

	void InitSwapchainParamaters();

	void InitCommandPool();


	VkCommandPool GetVkGraphicCommandPool() const;
	VkCommandPool GetVkComputeCommandPool() const;
	VkCommandPool GetVkTransferCommandPool() const;

public:
	VkInstance										m_VkInstance;
	VkPhysicalDevice								m_VkPhysicalDevice;
	const VulkanDevice&								m_Device;
	VkSurfaceKHR									m_VkSurface = VK_NULL_HANDLE;
	VkSurfaceCapabilitiesKHR						m_VkSurfaceCapabilities;
	VkSurfaceFormatKHR								m_VkSurfaceFormat;
	VkPresentModeKHR								m_VkPresentMode;
	VkFormat										m_VkSwapChainFormat;
	std::map<CommandPoolTypes, VkCommandPool>		m_VkCommandPools;
};

END_NAMESPACE_SPECTRE