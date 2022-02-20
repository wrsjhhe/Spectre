#pragma once

BEGIN_NAMESPACE_SPECTRE

class VulkanDescriptorPool : public Noncopyable
{
public:
	static constexpr uint32_t DESCRIPTOR_TYPE_COUNT = 3;

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
	uint32_t                     m_DescriptorCount = 0;
};

END_NAMESPACE_SPECTRE