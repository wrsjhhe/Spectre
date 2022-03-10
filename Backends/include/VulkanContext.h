#pragma once
#include <map>
#include "RenderTypes.h"
#include "VulkanEngine.h"
BEGIN_NAMESPACE_SPECTRE

class VulkanContext
{
public:
	VulkanContext(VulkanEngine* pEngine);
	~VulkanContext();

	void SetVertexDesc(const std::vector<VertexAttribute>& attrs);

	void CalcSwapChainExtent(uint32_t& width, uint32_t& height);

	void CalcSwapchainParamaters(VkSurfaceKHR surface);
	void ReCalcSwapchainParamaters();
	void InitCommandPool();


	VkCommandPool GetVkGraphicCommandPool() const;
	VkCommandPool GetVkComputeCommandPool() const;
	VkCommandPool GetVkTransferCommandPool() const;

	const std::vector<VkVertexInputAttributeDescription>& GetInputAttributes()
	{
		return m_InputAttributeDesc;
	}
	const std::vector<VkVertexInputBindingDescription>& GetInputBinding() const
	{
		return m_InputBindingDesc;
	}

public:
	VulkanEngine*									m_VulkanEnginePtr;
	VkSurfaceKHR									m_VkSurface = VK_NULL_HANDLE;
	VkSurfaceCapabilitiesKHR						m_VkSurfaceCapabilities;
	VkSurfaceFormatKHR								m_VkSurfaceFormat;
	VkPresentModeKHR								m_VkPresentMode;
	VkFormat										m_VkSwapChainFormat;
	std::map<CommandPoolTypes, VkCommandPool>		m_VkCommandPools;
	std::vector<VkVertexInputBindingDescription>	m_InputBindingDesc;
	std::vector<VkVertexInputAttributeDescription>  m_InputAttributeDesc;
	std::vector<std::vector<uint32_t>>				m_VertexShaders;
	std::vector<std::vector<uint32_t>>				m_FragmentShaders;
	
};

END_NAMESPACE_SPECTRE