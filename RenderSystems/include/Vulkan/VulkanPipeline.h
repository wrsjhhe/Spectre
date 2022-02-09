#pragma once

BEGIN_NAMESPACE_SPECTRE

class VulkanPipeline
{
public:
	static std::shared_ptr<VulkanPipeline> CreatePipeline(const VulkanDevice& vulkanDevice,const VulkanRenderPass& renderPass, const VulkanDescriptorSetLayout& descriptorSetLayout);
public:
	~VulkanPipeline();

	VkPipeline GetVkPipeline() { return m_VkPipeline; }
	VkPipelineLayout GetVkPipelineLayout() { return m_VkPipelineLayout; }
	void Destory();
private:
	VulkanPipeline(const VulkanDevice& vulkanDevice, const VulkanRenderPass& renderPass, const VulkanDescriptorSetLayout& descriptorSetLayout);
	VkShaderModule LoadSPIPVShader(const std::string& filepath);
private:
	const VulkanDevice&					m_Device;
	VkPipelineLayout					m_VkPipelineLayout = VK_NULL_HANDLE;
	VkPipeline							m_VkPipeline = VK_NULL_HANDLE;
	VkPipelineCache						m_VkPipelineCache = VK_NULL_HANDLE;
};

END_NAMESPACE_SPECTRE