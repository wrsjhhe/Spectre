#pragma once

BEGIN_NAMESPACE_SPECTRE

class VulkanDescriptorPool
{
public:
	static std::shared_ptr<VulkanDescriptorPool> CreateDescriptorPool(const VulkanDevice& vulkanDevice);
public:
	~VulkanDescriptorPool();

	const VkDescriptorPool& GetVkDescriptorPool() const { return m_VkDescriptorPool; }

	void Destory();
private:
	VulkanDescriptorPool(const VulkanDevice& vulkanDevice);

private:
	const VulkanDevice&			 m_Device;
	VkDescriptorPool             m_VkDescriptorPool = VK_NULL_HANDLE;
};

END_NAMESPACE_SPECTRE