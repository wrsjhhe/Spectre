#pragma once

BEGIN_NAMESPACE_SPECTRE

class VulkanDescriptorSetLayout : public Noncopyable
{
public:
	static std::shared_ptr<VulkanDescriptorSetLayout> CreateDescriptorSetLayout(const VulkanDevice& vulkanDevice);
public:
	~VulkanDescriptorSetLayout();

	const VkDescriptorSetLayout& GetVkDescriptorSetLayout() const { return m_VkDescriptorSetLayout; }

	void Destory();
private:
	VulkanDescriptorSetLayout(const VulkanDevice& vulkanDevice);

private:
	const VulkanDevice&				m_Device;
	VkDescriptorSetLayout           m_VkDescriptorSetLayout = VK_NULL_HANDLE;
};

END_NAMESPACE_SPECTRE