#include "VulkanCommon.h"
#include "VulkanDevice.h"
#include "VulkanRenderPass.h"
#include "VulkanPipelineCache.h"

#include <fstream>
static std::vector<char> readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

USING_NAMESPACE(Spectre)

std::shared_ptr<VulkanPipelineCache> VulkanPipelineCache::Create(VkDevice device)
{
	auto* pPipeline = new VulkanPipelineCache(device);
	return std::shared_ptr<VulkanPipelineCache>(pPipeline);
}

VulkanPipelineCache::~VulkanPipelineCache()
{
	Destory();
}

void VulkanPipelineCache::SetVertexDescription(const std::vector<VkVertexInputBindingDescription>& bindingDescriptions,
	const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions)
{
	m_BindingDescriptions = bindingDescriptions;
	m_AttributeDescriptions = attributeDescriptions;

	m_VertexInputState = {};
	m_VertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	m_VertexInputState.vertexBindingDescriptionCount = m_BindingDescriptions.size();
	m_VertexInputState.pVertexBindingDescriptions = m_BindingDescriptions.data();
	m_VertexInputState.vertexAttributeDescriptionCount = m_AttributeDescriptions.size();
	m_VertexInputState.pVertexAttributeDescriptions = m_AttributeDescriptions.data();
}

void VulkanPipelineCache::CreateShaderModules(const std::vector<std::vector<uint32_t>>& vertexShaders,
	const std::vector<std::vector<uint32_t>>& fragmentShaders)
{
	uint32_t offset = 0;
	m_ShaderStages.resize(vertexShaders.size() + fragmentShaders.size());
	for (uint32_t i = offset; i < vertexShaders.size(); ++i)
	{
		m_ShaderStages[i] = {};
		m_ShaderStages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		m_ShaderStages[i].stage = VK_SHADER_STAGE_VERTEX_BIT;
		m_ShaderStages[i].module = LoadSPIPVShader(vertexShaders[i]);
		m_ShaderStages[i].pName = "main";
		++offset;
	}
	
	for (uint32_t i = 0; i < fragmentShaders.size(); ++i)
	{
		m_ShaderStages[offset] = {};
		m_ShaderStages[offset].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		m_ShaderStages[offset].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		m_ShaderStages[offset].module = LoadSPIPVShader(fragmentShaders[i]);
		m_ShaderStages[offset].pName = "main";
		++offset;
	}
}

void VulkanPipelineCache::CreatePipelineInstance(const VulkanRenderPass& renderPass)
{
	if (m_VkPipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(m_VkDevice, m_VkPipeline, nullptr);
		m_VkPipeline = nullptr;
	}

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
	vkCreateGraphicsPipelines(m_VkDevice, m_VkPipelineCache, 1, &pipelineCreateInfo, nullptr, &m_VkPipeline);
	
	for (auto& shaderStage : m_ShaderStages)
	{
		vkDestroyShaderModule(m_VkDevice, shaderStage.module, nullptr);
	}
}
void VulkanPipelineCache::BindDescriptorSets(const VkCommandBuffer& commandBuffer)
{
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VkPipelineLayout, 0, 1, &m_VkDescriptorSet, 0, nullptr);
}

void VulkanPipelineCache::BindPipeline(const VkCommandBuffer& commandBuffer)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VkPipeline);
}

void VulkanPipelineCache::Destory()
{
	VkDevice device = m_VkDevice;
	vkDestroyPipelineLayout(device, m_VkPipelineLayout, nullptr);
	vkDestroyPipeline(device, m_VkPipeline, nullptr);
	vkDestroyPipelineCache(device, m_VkPipelineCache, nullptr);

	vkDestroyDescriptorSetLayout(device, m_VkDescriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(device, m_VkDescriptorPool, nullptr);
}

VulkanPipelineCache::VulkanPipelineCache(VkDevice device):
	m_VkDevice(device)
{
	CreateDescriptorPool();
	CreatePipelineLayout();


	VkPipelineLayoutCreateInfo pipeLayoutInfo{};
	pipeLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeLayoutInfo.setLayoutCount = 1;
	pipeLayoutInfo.pSetLayouts = &m_VkDescriptorSetLayout;
	vkCreatePipelineLayout(device, &pipeLayoutInfo, nullptr, &m_VkPipelineLayout);

	VkPipelineCacheCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	vkCreatePipelineCache(device, &createInfo, nullptr, &m_VkPipelineCache);

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

VkShaderModule VulkanPipelineCache::LoadSPIPVShader(const std::vector<uint32_t>& shaderCode)
{
	VkShaderModuleCreateInfo moduleCreateInfo{};
	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.codeSize = shaderCode.size()*sizeof(uint32_t);
	moduleCreateInfo.pCode = shaderCode.data();

	VkShaderModule shaderModule;
	vkCreateShaderModule(m_VkDevice, &moduleCreateInfo, nullptr, &shaderModule);

	return shaderModule;
}

void VulkanPipelineCache::CreateDescriptorPool()
{
	VkDescriptorPoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = DESCRIPTOR_TYPE_COUNT;

	VkDescriptorPoolCreateInfo descriptorPoolInfo{};
	descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.poolSizeCount = 1;
	descriptorPoolInfo.pPoolSizes = &poolSize;
	descriptorPoolInfo.maxSets = 1;
	vkCreateDescriptorPool(m_VkDevice, &descriptorPoolInfo, nullptr, &m_VkDescriptorPool);
}

void VulkanPipelineCache::CreatePipelineLayout()
{
	VkDescriptorSetLayoutBinding layoutBinding;
	layoutBinding.binding = 0;
	layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBinding.descriptorCount = 1;
	layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	layoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo descSetLayoutInfo{};
	descSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descSetLayoutInfo.bindingCount = 1;
	descSetLayoutInfo.pBindings = &layoutBinding;
	vkCreateDescriptorSetLayout(m_VkDevice, &descSetLayoutInfo, nullptr, &m_VkDescriptorSetLayout);
}

void VulkanPipelineCache::CreateDescriptorSet(VkDescriptorBufferInfo  MVPDescriptor)
{
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_VkDescriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &m_VkDescriptorSetLayout;
	vkAllocateDescriptorSets(m_VkDevice, &allocInfo, &m_VkDescriptorSet);

	VkWriteDescriptorSet writeDescriptorSet{};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.dstSet = m_VkDescriptorSet;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeDescriptorSet.pBufferInfo = &MVPDescriptor;
	writeDescriptorSet.dstBinding = 0;
	vkUpdateDescriptorSets(m_VkDevice, 1, &writeDescriptorSet, 0, nullptr);
}