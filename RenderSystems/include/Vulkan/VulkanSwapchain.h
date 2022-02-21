#pragma once
#include "NativeWindow.h"
#include "VulkanImages.h"
#include "VulkanGraphicTypes.h"
#include "VulkanContext.h"
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
public:
	VulkanSwapChain(const VulkanDevice& device, const VulkanContext& context,const SwapChainDesc& desc);
	~VulkanSwapChain();

	VkSwapchainKHR GetVkSwapChain() const { return m_VkSwapChain; }

	uint32_t GetImageCount() const { return m_ImageCount; }

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }

	const VulkanImages& GetImages() const { return *m_Images; }

	VkFormat GetSwapChainFormat() const { return m_Context.m_VkSwapChainFormat; }

	uint32_t AcquireImageIndex(std::shared_ptr<VulkanSemaphore>& outSemaphore);

	SwapStatus Present(VkQueue presentQueue, VkSemaphore* doneSemaphore);
private:
	const VulkanDevice&								m_Device;
	const VulkanContext&							m_Context;
	VkSwapchainKHR									m_VkSwapChain;
	uint32_t										m_ImageCount;
	std::shared_ptr<VulkanImages>					m_Images;
	uint32_t										m_Width;
	uint32_t										m_Height;
	uint32_t										m_SemaphoreIndex = 0;
	std::vector<std::shared_ptr<VulkanSemaphore>>	m_ImageAcquiredSemaphore;
	uint32_t										m_CurrentImageIndex;
};

END_NAMESPACE_SPECTRE