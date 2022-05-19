#pragma once
#include "VulkanEngine.h"
#include "VulkanImage.h"
#include "VulkanSemaphore.h"

BEGIN_NAMESPACE_SPECTRE

struct SwapChainDesc
{
	uint32_t                  Width = 0;
	uint32_t                  Height = 0;
};

class VulkanSemaphore;

class VulkanSwapChain;
typedef std::shared_ptr<VulkanSwapChain> VulkanSwapChainPtr;
class VulkanSwapChain
{
public:
	enum class SwapStatus
	{
		Healthy = 0,
		OutOfDate = -1,
		SurfaceLost = -2,
	};
public:
	VulkanSwapChain(VkSurfaceKHR surface,const SwapChainDesc& desc);
	~VulkanSwapChain();

	VkSwapchainKHR GetVkSwapChain() const { return m_VkSwapChain; }

	uint32_t GetImageCount() const { return m_ImageCount; }

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }

	//const std::vector<VulkanImagePtr>& GetImages() const { return m_Images; }

	const std::vector<VulkanImageViewPtr> GetImageView() const { return m_SwapChainImageViews; }

	//VkFormat GetSwapChainFormat() const { return m_Context.m_VkSwapChainFormat; }

	uint32_t AcquireImageIndex(std::shared_ptr<VulkanSemaphore>& outSemaphore);

	SwapStatus Present(VkQueue presentQueue, VkSemaphore* doneSemaphore);
private:
	//const VulkanContext&							m_Context;
	VkSwapchainKHR									m_VkSwapChain;
	uint32_t										m_ImageCount;
	//std::vector<VulkanImagePtr>						m_Images;
	uint32_t										m_Width;
	uint32_t										m_Height;
	uint32_t										m_SemaphoreIndex = 0;
	std::vector<std::shared_ptr<VulkanSemaphore>>	m_ImageAcquiredSemaphore;
	uint32_t										m_CurrentImageIndex;
	std::vector<VulkanImageViewPtr>					m_SwapChainImageViews;
};

END_NAMESPACE_SPECTRE