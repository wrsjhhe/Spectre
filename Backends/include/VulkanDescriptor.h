#pragma once
#include "VulkanEngine.h"

BEGIN_NAMESPACE_SPECTRE



class VulkanDescriptorBuilder
{
public:
	void AddBind(uint32_t binding,VkDescriptorType type, VkShaderStageFlags stageFlags);

	VkDescriptorSet Build(VkDescriptorBufferInfo* bufferInfo);

	VkDescriptorSetLayout GetOrCreateLayout();

private:
	std::vector<VkDescriptorSetLayoutBinding> m_Bindings;
	VkDescriptorSetLayout m_Layout = VK_NULL_HANDLE;
};

END_NAMESPACE_SPECTRE
