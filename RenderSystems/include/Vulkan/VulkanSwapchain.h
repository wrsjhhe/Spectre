#pragma once
#include "NativeWindow.h"
#include "VulkanImages.h"
#include "VulkanGraphicTypes.h"
BEGIN_NAMESPACE_SPECTRE

class VulkanSurface;
class VulkanSemaphore;

class VulkanSwapChain : public Noncopyable
{
public:
	enum class SwapStatus
	{
		Healthy = 0,
		OutOfDate = -1,
		SurfaceLost = -2,
	};
private:

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

public:
	VulkanSwapChain(const VulkanInstance& vulkanInstance,const VulkanDevice& vulkanDevice,
		const VulkanSurface& vulkanSurface, const SwapChainDesc& desc);
	~VulkanSwapChain();

	VkSwapchainKHR GetVkSwapChain() const { return m_VkSwapChain; }

	uint32_t GetImageCount() const { return m_ImageCount; }

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }
	VkFormat GetSwapChainFormat() const { return m_VkSwapChainFormat; }

	const VulkanImages& GetImages() const { return *m_Images; }

	uint32_t AcquireImageIndex(std::shared_ptr<VulkanSemaphore>& outSemaphore);

	SwapStatus Present(VkQueue presentQueue, VkSemaphore* doneSemaphore);

private:

	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
private:

	const VulkanInstance&							m_Instance;
	const VulkanDevice&								m_Device;
	const VulkanSurface&							m_Surface;
	VkSwapchainKHR									m_VkSwapChain;
	uint32_t										m_ImageCount;
	std::shared_ptr<VulkanImages>					m_Images;
	VkFormat										m_VkSwapChainFormat;
	uint32_t										m_Width;
	uint32_t										m_Height;
	uint32_t										m_SemaphoreIndex = 0;
	std::vector<std::shared_ptr<VulkanSemaphore>>	m_ImageAcquiredSemaphore;
	uint32_t										m_CurrentImageIndex;
};

END_NAMESPACE_SPECTRE