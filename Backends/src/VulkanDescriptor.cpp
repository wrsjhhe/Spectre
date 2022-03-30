#include "VulkanDescriptor.h"


USING_NAMESPACE(Spectre)

VulkanDescriptorSet::VulkanDescriptorSet(VkDescriptorSet set):
	m_VkDescriptorSet(set)
{

}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
	VulkanEngine::GetInstance()->GetDescriptorAllocator().Free(&m_VkDescriptorSet);
}

void VulkanDescriptorBuilder::AddBind(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stageFlags)
{
	VkDescriptorSetLayoutBinding newBinding{};

	newBinding.descriptorCount = 1;
	newBinding.descriptorType = type;
	newBinding.pImmutableSamplers = nullptr;
	newBinding.stageFlags = stageFlags;
	newBinding.binding = binding;

	m_Bindings.push_back(newBinding);
}


VulkanDescriptorSetPtr VulkanDescriptorBuilder::Build(VkDescriptorBufferInfo* bufferInfo)
{
	m_Layout = GetOrCreateLayout();
	std::vector<VkWriteDescriptorSet> writes;
	for(auto& binding: m_Bindings)
	{
		VkWriteDescriptorSet newWrite{};
		newWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		newWrite.pNext = nullptr;

		newWrite.descriptorCount = 1;
		newWrite.descriptorType = binding.descriptorType;
		newWrite.pBufferInfo = bufferInfo;
		newWrite.dstBinding = binding.binding;

		writes.push_back(newWrite);
	}
	
	VkDescriptorSet set = VK_NULL_HANDLE;
	VulkanEngine::GetInstance()->GetDescriptorAllocator().Allocate(&set, m_Layout);

	for (VkWriteDescriptorSet& w : writes) {
		w.dstSet = set;
	}

	vkUpdateDescriptorSets(VulkanEngine::GetInstance()->GetVkDevice(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);

	return std::make_shared<VulkanDescriptorSet>(set);
}

VkDescriptorSetLayout VulkanDescriptorBuilder::GetOrCreateLayout()
{
	if (m_Layout != VK_NULL_HANDLE)
	{
		return m_Layout;
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.pNext = nullptr;

	layoutInfo.pBindings = m_Bindings.data();
	layoutInfo.bindingCount = static_cast<uint32_t>(m_Bindings.size());

	m_Layout = VulkanEngine::GetInstance()->GetDescriptorLayoutCache().CreateDescriptorLayout(&layoutInfo);

	return m_Layout;
}


