#pragma once
#include "NativeWindow.h"
#include "VulkanImages.h"

BEGIN_NAMESPACE_SPECTRE

class VulkanInstance;
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
	VulkanSwapChain(std::shared_ptr<const VulkanInstance> vulkanInstance, 
		std::shared_ptr<const VulkanDevice> vulkanDevice,
		const NativeWindow& window, uint32_t width,uint32_t height);
	~VulkanSwapChain();

	VkSwapchainKHR GetVkSwapChain() const { return m_VkSwapChain; }

	uint32_t GetImageCount() const { return m_ImageCount; }

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }
	VkFormat GetSwapChainFormat() const { return m_VkSwapChainFormat; }

	const VulkanImages& GetImages() const { return *m_Images; }

	uint32_t AcquireImageIndex(std::shared_ptr<VulkanSemaphore>& outSemaphore);

	SwapStatus Present(VkQueue presentQueue, VkSemaphore* doneSemaphore);

	void DestorySwapChain();
private:
	void CreateSurface();

	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
private:

	std::shared_ptr<const VulkanInstance>			m_VulkanInstance;
	std::shared_ptr<const VulkanDevice>				m_VulkanDevice;
	VkSwapchainKHR									m_VkSwapChain;
	VkSurfaceKHR									m_VkSurface = VK_NULL_HANDLE;
	NativeWindow									m_Window;
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