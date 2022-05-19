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


VulkanDescriptorSetPtr VulkanDescriptorBuilder::Build(std::vector<VkDescriptorBufferInfo*> bufferInfos, 
	std::vector<VkDescriptorImageInfo*> imageInfos)
{
	EXP_CHECK(bufferInfos.size()+ imageInfos.size() <= m_Bindings.size(), "bufferInfos's size need equal to bindinds's size");
	m_Layout = GetOrCreateLayout();
	std::vector<VkWriteDescriptorSet> writes;

	uint32_t index = 0;
	for (; index < bufferInfos.size(); ++index)
	{
		VkWriteDescriptorSet newWrite{};
		newWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		newWrite.pNext = nullptr;

		newWrite.descriptorCount = 1;
		newWrite.descriptorType = m_Bindings[index].descriptorType;
		newWrite.pBufferInfo = bufferInfos[index];
		newWrite.dstBinding = m_Bindings[index].binding;

		writes.push_back(newWrite);
	}
	
	for (uint32_t i = 0;i < imageInfos.size();++i)
	{
		VkWriteDescriptorSet newWrite{};
		newWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		newWrite.pNext = nullptr;

		newWrite.descriptorCount = 1;
		newWrite.descriptorType = m_Bindings[i+index].descriptorType;
		newWrite.pImageInfo = imageInfos[i];
		newWrite.dstBinding = m_Bindings[i + index].binding;;

		writes.push_back(newWrite);
	}

	VkDescriptorSet set = VK_NULL_HANDLE;
	VulkanEngine::GetInstance()->GetDescriptorAllocator().Allocate(&set, m_Layout);

	for (VkWriteDescriptorSet& w : writes) 
	{
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


