#pragma once
#include <memory>
BEGIN_NAMESPACE_SPECTRE

class VulkanSemaphore
{
public:
	static std::shared_ptr<VulkanSemaphore> CreateSemaphore();
public:
	~VulkanSemaphore();

	VkSemaphore& GetVkSemaphore() { return m_VkSemaphore; }

	void Destory();
private:
	VulkanSemaphore();
private:
	VkSemaphore							m_VkSemaphore = VK_NULL_HANDLE;
};

END_NAMESPACE_SPECTRE