#pragma once
#include <memory>
#include <vector>
BEGIN_NAMESPACE_SPECTRE

class VulkanDevice;

class VulkanCommandPool:std::enable_shared_from_this<VulkanCommandPool>
{
public:
	static std::shared_ptr<VulkanCommandPool> CreateCommandPool(const std::shared_ptr<const VulkanDevice>& vulkanDevice);
public:
	~VulkanCommandPool();

	std::shared_ptr<const VulkanCommandPool> GetSharedPtr() const
	{
		return shared_from_this();
	}
	//Todo 将每类CommandPool分开
	VkCommandPool GetVkCommandPool() const { return m_VkCommandPool; }

	void Destroy();
private:
	VulkanCommandPool(const std::shared_ptr<const VulkanDevice>& vulkanDevice);
private:
	std::shared_ptr<const VulkanDevice>			 m_DevicePtr;
	VkCommandPool								 m_VkCommandPool = VK_NULL_HANDLE;
};


END_NAMESPACE_SPECTRE