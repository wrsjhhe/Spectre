#pragma once

BEGIN_NAMESPACE_SPECTRE

class VulkanDescriptorSet
{
public:
	static std::shared_ptr<VulkanDescriptorSet> CreateDescriptorSet(const VulkanDevice& vulkanDevice,
		const VulkanDescriptorPool& descriptorPool,const VulkanDescriptorSetLayout& descriptorSetLayout);
public:
	const VkDescriptorSet& GetVkDescriptorSet() const { return m_VkDescriptorSet; }

private:
	VulkanDescriptorSet(const VulkanDevice& vulkanDevice,
		const VulkanDescriptorPool& descriptorPool, const VulkanDescriptorSetLayout& descriptorSetLayout);

private:
	//const VulkanDevice&					m_Device;
	//const VulkanDescriptorPool&			m_DescriptorPool;
	//const VulkanDescriptorSetLayout&	m_DescriptorSetLayout;
	VkDescriptorSet						m_VkDescriptorSet = VK_NULL_HANDLE;
};


END_NAMESPACE_SPECTRE