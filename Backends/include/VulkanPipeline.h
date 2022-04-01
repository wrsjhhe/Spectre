#pragma once

#include "SpectreApi.h"
#include "VulkanCommon.h"
#include <map>
#include <memory>
#include "RenderTypes.h"
#include "VulkanBuffer.h"


BEGIN_NAMESPACE_SPECTRE

class VulkanRenderPass;
class VulkanPipeline;
typedef std::shared_ptr<VulkanPipeline> VulkanPipelinePtr;
class VulkanPipeline : public SpectreApi
{
private:
	// Three descriptor set layouts: uniforms, combined image samplers, and input attachments.
	static constexpr uint32_t DESCRIPTOR_TYPE_COUNT = 3;

public:
	DefineClass(VulkanPipeline)

	static std::shared_ptr<VulkanPipeline> Create();
public:
	~VulkanPipeline();

	void SetVertexDescription(const std::vector<VertexAttribute>& VertexAttributes);

	void SetVertexShaders(const std::vector<std::string>& vertexShaders);

	void SetFragmentShaders(const std::vector<std::string>& fragmentShaders);

	void AddDescriptorSetLayout(VkDescriptorSetLayout layout);

	void CreatePipelineInstance(const VulkanRenderPass& renderPass);

	VkPipeline GetVkPipeline() { return m_VkPipeline; }
	VkPipelineLayout GetVkPipelineLayout() { return m_VkPipelineLayout; }

	void BindPipeline(const VkCommandBuffer& commandBuffer);

	void Destory();
private:
	VulkanPipeline();
	VkShaderModule LoadSPIPVShader(const std::string& shaderCode, ShaderType type);
	void CreateShaderModules();
private:
	VkPipelineLayout					m_VkPipelineLayout = VK_NULL_HANDLE;
	VkPipeline							m_VkPipeline = VK_NULL_HANDLE;

	std::vector<VkDescriptorSetLayout>  m_Layouts;

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

	std::vector<std::string>								m_VertexShaders;
	std::vector<std::string>								m_FragmentShaders;
};

END_NAMESPACE_SPECTRE