#pragma once
#include "VulkanEngine.h"

BEGIN_NAMESPACE_SPECTRE

class VulkanDescriptorSet
{
public:
	VulkanDescriptorSet(VkDescriptorSet set);
	~VulkanDescriptorSet();

	VkDescriptorSet& GetVkSet() {
		return m_VkDescriptorSet;
	}
private:
	VkDescriptorSet m_VkDescriptorSet;
};

typedef std::shared_ptr<VulkanDescriptorSet> VulkanDescriptorSetPtr;
class VulkanDescriptorBuilder
{
public:
	void AddBind(uint32_t binding,VkDescriptorType type, VkShaderStageFlags stageFlags);

	VulkanDescriptorSetPtr Build(std::vector<VkDescriptorBufferInfo*> bufferInfos,
		std::vector<VkDescriptorImageInfo*> imageInfos = std::vector<VkDescriptorImageInfo*>());

	VkDescriptorSetLayout GetOrCreateLayout();

private:
	std::vector<VkDescriptorSetLayoutBinding> m_Bindings;
	VkDescriptorSetLayout m_Layout = VK_NULL_HANDLE;
};

END_NAMESPACE_SPECTRE
