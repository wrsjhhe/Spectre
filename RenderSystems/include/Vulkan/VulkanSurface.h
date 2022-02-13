#pragma once
#include "NativeWindow.h"
BEGIN_NAMESPACE_SPECTRE

class VulkanSurface: public Noncopyable
{
public:
	static std::shared_ptr<VulkanSurface> CreateSurface(const VulkanInstance& vulkanInstance,const NativeWindow& wnd);

public:
	~VulkanSurface();

public: 
	VkSurfaceKHR GetVkSurface() const { return m_VkSurface; }

private:
	VulkanSurface(const VulkanInstance& vulkanDevice, const NativeWindow& wnd);
private:
	const VulkanInstance&       m_Instance;
	VkSurfaceKHR                m_VkSurface = VK_NULL_HANDLE;
	NativeWindow				m_Window;
};

END_NAMESPACE_SPECTRE