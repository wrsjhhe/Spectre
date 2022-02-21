#pragma once
#include "VulkanGraphicTypes.h"

BEGIN_NAMESPACE_SPECTRE



class VulkanContext
{
public:
	~VulkanContext();
	void Init();

	void CalculateSwapChainExtent(uint32_t& width, uint32_t& height);

public:
	VkInstance								m_VkInstance;
	VkPhysicalDevice						m_VkPhysicalDevice;
	VkSurfaceKHR							m_VkSurface = VK_NULL_HANDLE;
	VkSurfaceCapabilitiesKHR				m_VkSurfaceCapabilities;
	VkSurfaceFormatKHR                      m_VkSurfaceFormat;
	VkPresentModeKHR                        m_VkPresentMode;
	VkFormat								m_VkSwapChainFormat;
};

END_NAMESPACE_SPECTRE