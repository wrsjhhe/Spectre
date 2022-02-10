#pragma once
#include <memory>
#include <map>
#include <vulkan.h>
BEGIN_NAMESPACE_SPECTRE

class VulkanDevice;

class VulkanCommandPool:std::enable_shared_from_this<VulkanCommandPool>, public Noncopyable
{
public:
	static std::shared_ptr<VulkanCommandPool> CreateCommandPool(const VulkanDevice& vulkanDevice);
public:
	~VulkanCommandPool();

	std::shared_ptr<const VulkanCommandPool> GetSharedPtr() const
	{
		return shared_from_this();
	}

	VkCommandPool GetVkGraphicCommandPool() const;
	//VkCommandPool GetVkComputeCommandPool() const;
	//VkCommandPool GetVkTransferCommandPool() const;

	void Destroy();
private:
	VulkanCommandPool(const VulkanDevice& vulkanDevice);
private:
	const VulkanDevice&							 m_Device;
	std::map<uint32_t, VkCommandPool>			 m_VkCommandPools;
};


END_NAMESPACE_SPECTRE