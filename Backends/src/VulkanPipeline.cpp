#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"
#include "VulkanBuffer.h"
#include "ShaderTool.h"
#include "VulkanEngine.h"
#include <fstream>


USING_NAMESPACE(Spectre)


static uint32_t VertexAttributeToSize(VertexAttribute attribute)
{
	// count * sizeof(float)
	if (attribute == VertexAttribute::VertexAttribute_Position)
	{
		return 3 * sizeof(float);
	}
	else if (attribute == VertexAttribute::VertexAttribute_UV0)
	{
		return 2 * sizeof(float);
	}
	else if (attribute == VertexAttribute::VertexAttribute_UV1)
	{
		return 2 * sizeof(float);
	}
	else if (attribute == VertexAttribute::VertexAttribute_Normal)
	{
		return 3 * sizeof(float);
	}
	else if (attribute == VertexAttribute::VertexAttribute_Tangent)
	{
		return 4 * sizeof(float);
	}
	else if (attribute == VertexAttribute::VertexAttribute_Color)
	{
		return 3 * sizeof(float);
	}
	else if (attribute == VertexAttribute::VertexAttribute_SkinWeight)
	{
		return 4 * sizeof(float);
	}
	else if (attribute == VertexAttribute::VertexAttribute_SkinIndex)
	{
		return 4 * sizeof(float);
	}
	else if (attribute == VertexAttribute::VertexAttribute_SkinPack)
	{
		return 3 * sizeof(float);
	}
	else if (attribute == VertexAttribute::VertexAttribute_Custom0 ||
		attribute == VertexAttribute::VertexAttribute_Custom1 ||
		attribute == VertexAttribute::VertexAttribute_Custom2 ||
		attribute == VertexAttribute::VertexAttribute_Custom3
		)
	{
		return 4 * sizeof(float);
	}
	else if (attribute == VertexAttribute::VertexAttribute_InstanceFloat1)
	{
		return 1 * sizeof(float);
	}
	else if (attribute == VertexAttribute::VertexAttribute_InstanceFloat2)
	{
		return 2 * sizeof(float);
	}
	else if (attribute == VertexAttribute::VertexAttribute_InstanceFloat3)
	{
		return 3 * sizeof(float);
	}
	else if (attribute == VertexAttribute::VertexAttribute_InstanceFloat4)
	{
		return 4 * sizeof(float);
	}

	return 0;
}

static VkFormat VertexAttributeToVkFormat(VertexAttribute attribute)
{
	VkFormat format = VK_FORMAT_R32G32B32_SFLOAT;
	if (attribute == VertexAttribute::VertexAttribute_Position)
	{
		format = VK_FORMAT_R32G32B32_SFLOAT;
	}
	else if (attribute == VertexAttribute::VertexAttribute_UV0)
	{
		format = VK_FORMAT_R32G32_SFLOAT;
	}
	else if (attribute == VertexAttribute::VertexAttribute_UV1)
	{
		format = VK_FORMAT_R32G32_SFLOAT;
	}
	else if (attribute == VertexAttribute::VertexAttribute_Normal)
	{
		format = VK_FORMAT_R32G32B32_SFLOAT;
	}
	else if (attribute == VertexAttribute::VertexAttribute_Tangent)
	{
		format = VK_FORMAT_R32G32B32A32_SFLOAT;
	}
	else if (attribute == VertexAttribute::VertexAttribute_Color)
	{
		format = VK_FORMAT_R32G32B32_SFLOAT;
	}
	else if (attribute == VertexAttribute::VertexAttribute_SkinPack)
	{
		format = VK_FORMAT_R32G32B32_SFLOAT;
	}
	else if (attribute == VertexAttribute::VertexAttribute_SkinWeight)
	{
		format = VK_FORMAT_R32G32B32A32_SFLOAT;
	}
	else if (attribute == VertexAttribute::VertexAttribute_SkinIndex)
	{
		format = VK_FORMAT_R32G32B32A32_SFLOAT;
	}
	else if (attribute == VertexAttribute::VertexAttribute_Custom0 ||
		attribute == VertexAttribute::VertexAttribute_Custom1 ||
		attribute == VertexAttribute::VertexAttribute_Custom2 ||
		attribute == VertexAttribute::VertexAttribute_Custom3
		)
	{
		format = VK_FORMAT_R32G32B32A32_SFLOAT;
	}
	else if (attribute == VertexAttribute::VertexAttribute_InstanceFloat1)
	{
		format = VK_FORMAT_R32_SFLOAT;
	}
	else if (attribute == VertexAttribute::VertexAttribute_InstanceFloat2)
	{
		format = VK_FORMAT_R32G32_SFLOAT;
	}
	else if (attribute == VertexAttribute::VertexAttribute_InstanceFloat3)
	{
		format = VK_FORMAT_R32G32B32_SFLOAT;
	}
	else if (attribute == VertexAttribute::VertexAttribute_InstanceFloat4)
	{
		format = VK_FORMAT_R32G32B32A32_SFLOAT;
	}

	return format;
}


std::shared_ptr<VulkanPipeline> VulkanPipeline::Create()
{
	auto* pPipeline = new VulkanPipeline();
	return std::shared_ptr<VulkanPipeline>(pPipeline);
}

VulkanPipeline::~VulkanPipeline()
{
	Destory();
}
void VulkanPipeline::SetVertexDescription(const std::vector<VertexAttribute>& VertexAttributes)
{
	std::vector<VkVertexInputBindingDescription> bindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

	uint32_t stride = 0;
	for (uint32_t i = 0; i < VertexAttributes.size(); ++i) 
	{
		stride += VertexAttributeToSize(VertexAttributes[i]);
	}

	m_BindingDescriptions.resize(1);
	m_BindingDescriptions[0].binding = 0;
	m_BindingDescriptions[0].stride = stride;
	m_BindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	//create VertexInputAttributeDescription
	m_AttributeDescriptions.resize(VertexAttributes.size());
	uint32_t offset = 0;
	for (uint32_t i = 0; i < VertexAttributes.size(); ++i)
	{
		m_AttributeDescriptions[i].binding = 0;
		m_AttributeDescriptions[i].location = i;
		m_AttributeDescriptions[i].format = VertexAttributeToVkFormat(VertexAttributes[i]);
		m_AttributeDescriptions[i].offset = offset;
		offset += VertexAttributeToSize(VertexAttributes[i]);
	}

	m_VertexInputState = {};
	m_VertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	m_VertexInputState.vertexBindingDescriptionCount = m_BindingDescriptions.size();
	m_VertexInputState.pVertexBindingDescriptions = m_BindingDescriptions.data();
	m_VertexInputState.vertexAttributeDescriptionCount = m_AttributeDescriptions.size();
	m_VertexInputState.pVertexAttributeDescriptions = m_AttributeDescriptions.data();
}

void VulkanPipeline::CreateShaderModules()
{
	uint32_t offset = 0;
	m_ShaderStages.resize(m_VertexShaders.size() + m_FragmentShaders.size());
	for (uint32_t i = offset; i < m_VertexShaders.size(); ++i)
	{
		m_ShaderStages[i] = {};
		m_ShaderStages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		m_ShaderStages[i].stage = VK_SHADER_STAGE_VERTEX_BIT;
		m_ShaderStages[i].module = LoadSPIPVShader(m_VertexShaders[i],ShaderType_Vertex);
		m_ShaderStages[i].pName = "main";
		++offset;
	}
	
	for (uint32_t i = 0; i < m_FragmentShaders.size(); ++i)
	{
		m_ShaderStages[offset] = {};
		m_ShaderStages[offset].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		m_ShaderStages[offset].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		m_ShaderStages[offset].module = LoadSPIPVShader(m_FragmentShaders[i], ShaderType_Fragment);
		m_ShaderStages[offset].pName = "main";
		++offset;
	}
}

void VulkanPipeline::SetVertexShaders(const std::vector<std::string>& vertexShaders)
{
	m_VertexShaders = vertexShaders;
}

void VulkanPipeline::SetFragmentShaders(const std::vector<std::string>& fragmentShaders)
{
	m_FragmentShaders = fragmentShaders;
}

void VulkanPipeline::CreatePipelineInstance(const VulkanRenderPass& renderPass)
{
	VkDevice device = VulkanEngine::GetInstance()->GetVkDevice();
	if (m_VkPipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(device, m_VkPipeline, nullptr);
		m_VkPipeline = nullptr;
	}

	CreateShaderModules();

	VkPipelineLayoutCreateInfo pipeLayoutInfo{};
	pipeLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeLayoutInfo.setLayoutCount = m_Layouts.size();
	pipeLayoutInfo.pSetLayouts = m_Layouts.data();
	vkCreatePipelineLayout(device, &pipeLayoutInfo, nullptr, &m_VkPipelineLayout);

	VkPipelineColorBlendStateCreateInfo colorBlendState = {};
	colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendState.attachmentCount = m_BlendAttachmentStates.size();
	colorBlendState.pAttachments = m_BlendAttachmentStates.data();

	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = (uint32_t)m_DynamicStateEnables.size();
	dynamicState.pDynamicStates = m_DynamicStateEnables.data();

	VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.layout = m_VkPipelineLayout;
	pipelineCreateInfo.renderPass = renderPass.GetVkRenderPass();
	pipelineCreateInfo.stageCount = (uint32_t)m_ShaderStages.size();
	pipelineCreateInfo.pStages = m_ShaderStages.data();
	pipelineCreateInfo.pVertexInputState = &m_VertexInputState;
	pipelineCreateInfo.pInputAssemblyState = &m_InputAssemblyState;
	pipelineCreateInfo.pRasterizationState = &m_RasterizationState;
	pipelineCreateInfo.pColorBlendState = &colorBlendState;
	pipelineCreateInfo.pMultisampleState = &m_MultisampleState;
	pipelineCreateInfo.pViewportState = &m_ViewportState;
	pipelineCreateInfo.pDepthStencilState = &m_DepthStencilState;
	pipelineCreateInfo.pDynamicState = &dynamicState;
	vkCreateGraphicsPipelines(device, VulkanEngine::GetInstance()->GetVkPipelineCache(), 1, &pipelineCreateInfo, nullptr, &m_VkPipeline);

	for (auto& shaderStage : m_ShaderStages)
	{
		vkDestroyShaderModule(device, shaderStage.module, nullptr);
	}
}


void VulkanPipeline::BindPipeline(const VkCommandBuffer& commandBuffer)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VkPipeline);
}

void VulkanPipeline::Destory()
{
	VkDevice device = VulkanEngine::GetInstance()->GetVkDevice();

	vkDestroyPipelineLayout(device, m_VkPipelineLayout, nullptr);
	vkDestroyPipeline(device, m_VkPipeline, nullptr);
}

VulkanPipeline::VulkanPipeline()
{
	VkDevice device = VulkanEngine::GetInstance()->GetVkDevice();

	m_InputAssemblyState = {};
	m_InputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	m_InputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	m_RasterizationState = {};
	m_RasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	m_RasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
	m_RasterizationState.cullMode = VK_CULL_MODE_NONE;
	m_RasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	m_RasterizationState.depthClampEnable = VK_FALSE;
	m_RasterizationState.rasterizerDiscardEnable = VK_FALSE;
	m_RasterizationState.depthBiasEnable = VK_FALSE;
	m_RasterizationState.lineWidth = 1.0f;

	m_BlendAttachmentStates.resize(1);
	m_BlendAttachmentStates[0].colorWriteMask = (
		VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT
		);
	m_BlendAttachmentStates[0].blendEnable = VK_FALSE;

	m_ViewportState = {};
	m_ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	m_ViewportState.viewportCount = 1;
	m_ViewportState.scissorCount = 1;

	m_DynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);
	m_DynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);
	

	m_DepthStencilState = {};
	m_DepthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	m_DepthStencilState.depthTestEnable = VK_TRUE;
	m_DepthStencilState.depthWriteEnable = VK_TRUE;
	m_DepthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	m_DepthStencilState.depthBoundsTestEnable = VK_FALSE;
	m_DepthStencilState.back.failOp = VK_STENCIL_OP_KEEP;
	m_DepthStencilState.back.passOp = VK_STENCIL_OP_KEEP;
	m_DepthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
	m_DepthStencilState.stencilTestEnable = VK_FALSE;
	m_DepthStencilState.front = m_DepthStencilState.back;

	m_MultisampleState = {};
	m_MultisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	m_MultisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; //todo
	m_MultisampleState.pSampleMask = nullptr;
}

VkShaderModule VulkanPipeline::LoadSPIPVShader(const std::string& shaderCode, ShaderType type)
{
	VkDevice device = VulkanEngine::GetInstance()->GetVkDevice();
	std::vector<uint32_t> spvCode = ShaderTool::Compile_glsl(shaderCode, type);

	VkShaderModuleCreateInfo moduleCreateInfo{};
	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.codeSize = spvCode.size()*sizeof(uint32_t);
	moduleCreateInfo.pCode = spvCode.data();

	VkShaderModule shaderModule;
	vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &shaderModule);

	return shaderModule;
}


void VulkanPipeline::AddDescriptorSetLayout(VkDescriptorSetLayout layout)
{
	m_Layouts.push_back(layout);
}
