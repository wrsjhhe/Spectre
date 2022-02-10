#pragma once
#include <memory>
BEGIN_NAMESPACE_SPECTRE

class VulkanSemaphore : public Noncopyable
{
public:
	static std::shared_ptr<VulkanSemaphore> CreateSemaphore(const VulkanDevice& vulkanDevice);
public:
	~VulkanSemaphore();

	VkSemaphore& GetVkSemaphore() { return m_VkSemaphore; }

	void Destory();
private:
	VulkanSemaphore(const VulkanDevice& vulkanDevice);
private:
	const VulkanDevice&					m_Device;
	VkSemaphore							m_VkSemaphore = VK_NULL_HANDLE;
};

END_NAMESPACE_SPECTRE