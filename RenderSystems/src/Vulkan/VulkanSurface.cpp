#include "VulkanCommon.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"

USING_NAMESPACE(Spectre)

std::shared_ptr<VulkanSurface> VulkanSurface::CreateSurface(const VulkanInstance& vulkanInstance, const NativeWindow& wnd)
{
	auto* pSurface = new VulkanSurface(vulkanInstance, wnd);
	return std::shared_ptr<VulkanSurface>(pSurface);
}

VulkanSurface::~VulkanSurface()
{
	vkDestroySurfaceKHR(m_Instance.GetVkInstance(), m_VkSurface, nullptr);
}

VulkanSurface::VulkanSurface(const VulkanInstance& vulkanInstance, const NativeWindow& wnd):
	m_Instance(vulkanInstance),
	m_Window(wnd)
{
	if (m_VkSurface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(m_Instance.GetVkInstance(), m_VkSurface, NULL);
		m_VkSurface = VK_NULL_HANDLE;
	}

	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hinstance = GetModuleHandle(NULL);
	surfaceCreateInfo.hwnd = (HWND)m_Window.hWnd;
	VkResult err = vkCreateWin32SurfaceKHR(m_Instance.GetVkInstance(), &surfaceCreateInfo, nullptr, &m_VkSurface);

	VK_CHECK(err, "Failed create surface!")
}
