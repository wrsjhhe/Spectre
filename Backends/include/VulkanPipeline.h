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

	static std::shared_ptr<VulkanPipeline> Create(VkDevice device);
public:
	~VulkanPipeline();

	void SetVertexDescription(const std::vector<VertexAttribute>& VertexAttributes);

	void CreateShaderModules(const std::vector<std::string>& vertexShaders, const std::vector<std::string>& fragmentShaders);

	void CreateUniformBuffer(uint32_t  bufferSize);

	std::shared_ptr<VulkanBuffer> GetUniformBuffer() { return m_MVPBuffer; }

	void CreatePipelineInstance(const VulkanRenderPass& renderPass);

	VkPipeline GetVkPipeline() { return m_VkPipeline; }
	VkPipelineLayout GetVkPipelineLayout() { return m_VkPipelineLayout; }

	void BindDescriptorSets(const VkCommandBuffer& commandBuffer);

	void BindPipeline(const VkCommandBuffer& commandBuffer);

	void Destory();
private:
	VulkanPipeline(VkDevice device);
	VkShaderModule LoadSPIPVShader(const std::string& shaderCode, ShaderType type);

	void CreateDescriptorPool();
	void CreateDescriptorSetLayout();


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

	std::vector<std::vector<uint32_t>>						m_VertexShaders;
	std::vector<std::vector<uint32_t>>						m_FragmentShaders;
	std::shared_ptr<VulkanBuffer>							m_MVPBuffer;
};

END_NAMESPACE_SPECTRE