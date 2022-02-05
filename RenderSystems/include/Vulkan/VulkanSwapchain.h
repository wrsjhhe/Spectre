#pragma once
#include "NativeWindow.h"

class VulkanInstance;

BEGIN_NAMESPACE_SPECTRE

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices 
{
	int graphicsFamily = -1;
	int presentFamily = -1;

	bool isComplete() {
		return graphicsFamily >= 0 && presentFamily >= 0;
	}
};

class VulkanSwapChain
{
public:
	VulkanSwapChain(std::shared_ptr<const VulkanInstance> vulkanInstance, 
		std::shared_ptr<const VulkanDevice> vulkanDevice,
		const NativeWindow& window, uint32_t width,uint32_t height);
	//void CreateSwapChain();

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }
	VkFormat GetSwapChainFormat() const { return m_VkSwapChainFormat; }
	const std::vector<VkImage> GetImages() const {return m_VkImages;}
	const std::vector<VkImageView> GetImageViews() const {return m_VkImageViews;}

private:
	void CreateSurface();

	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
private:

	std::shared_ptr<const VulkanInstance>   m_VulkanInstance;
	std::shared_ptr<const VulkanDevice>     m_VulkanDevice;
	VkSwapchainKHR							m_VkSwapChain;
	VkSurfaceKHR							m_VkSurface = VK_NULL_HANDLE;
	NativeWindow							m_Window;
	std::vector<VkImage>                    m_VkImages;
	std::vector<VkImageView>                m_VkImageViews;
	VkFormat                                m_VkSwapChainFormat;
	uint32_t                                m_Width;
	uint32_t                                m_Height;
};

END_NAMESPACE_SPECTRE