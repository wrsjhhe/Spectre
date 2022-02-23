#pragma once
#include <map>

BEGIN_NAMESPACE_SPECTRE

class VulkanPipelineCache : public Noncopyable
{
private:
	// Three descriptor set layouts: uniforms, combined image samplers, and input attachments.
	static constexpr uint32_t DESCRIPTOR_TYPE_COUNT = 3;

public:
	static std::shared_ptr<VulkanPipelineCache> Create(VkDevice device);
public:
	~VulkanPipelineCache();

	void SetVertexDescription(const std::vector<VkVertexInputBindingDescription>& bindingDescriptions,
		const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions);

	void CreatePipeline(const VulkanRenderPass& renderPass);

	VkPipeline GetVkPipeline() { return m_VkPipeline; }
	VkPipelineLayout GetVkPipelineLayout() { return m_VkPipelineLayout; }

	void BindDescriptorSets(const VkCommandBuffer& commandBuffer);

	void BindPipeline(const VkCommandBuffer& commandBuffer);

	void Destory();
private:
	VulkanPipelineCache(VkDevice device);
	VkShaderModule LoadSPIPVShader(const std::string& filepath);

	void CreateDescriptorPool();
	void CreatePipelineLayout();
	void CreateDescriptorSet();

private:
	VkDevice							m_VkDevice = VK_NULL_HANDLE;;
	VkPipelineLayout					m_VkPipelineLayout = VK_NULL_HANDLE;
	VkPipeline							m_VkPipeline = VK_NULL_HANDLE;
	VkPipelineCache						m_VkPipelineCache = VK_NULL_HANDLE;
	VkDescriptorPool					m_VkDescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout				m_VkDescriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorSet						m_VkDescriptorSet = VK_NULL_HANDLE;

	std::vector<VkVertexInputBindingDescription>			m_BindingDescriptions;
	std::vector<VkVertexInputAttributeDescription>          m_AttributeDescriptions;
	std::vector<VkPipelineShaderStageCreateInfo>			m_ShaderStages;
	VkPipelineVertexInputStateCreateInfo					m_VertexInputState;
	VkPipelineInputAssemblyStateCreateInfo					m_InputAssemblyState;
	VkPipelineRasterizationStateCreateInfo					m_RasterizationState;
	std::vector<VkPipelineColorBlendAttachmentState>		m_BlendAttachmentStates;
	VkPipelineMultisampleStateCreateInfo					m_MultisampleState;
	VkPipelineViewportStateCreateInfo						m_ViewportState;
	VkPipelineDepthStencilStateCreateInfo					m_DepthStencilState;
	std::vector<VkDynamicState>								m_DynamicStateEnables;
};

END_NAMESPACE_SPECTRE