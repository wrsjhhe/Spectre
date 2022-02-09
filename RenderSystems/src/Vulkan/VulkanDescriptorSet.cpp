#include "VulkanCommon.h"
#include "VulkanDevice.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanDescriptorSet.h"

USING_NAMESPACE(Spectre)

std::shared_ptr<VulkanDescriptorSet> VulkanDescriptorSet::CreateDescriptorSet(const VulkanDevice& vulkanDevice,
	const VulkanDescriptorPool& descriptorPool, const VulkanDescriptorSetLayout& descriptorSetLayout)
{
	auto* pDescriptorSet = new VulkanDescriptorSet(vulkanDevice, descriptorPool, descriptorSetLayout);
	return std::shared_ptr<VulkanDescriptorSet>(pDescriptorSet);
}


VulkanDescriptorSet::VulkanDescriptorSet(const VulkanDevice& vulkanDevice,
	const VulkanDescriptorPool& descriptorPool, const VulkanDescriptorSetLayout& descriptorSetLayout)
{
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool.GetVkDescriptorPool();
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &descriptorSetLayout.GetVkDescriptorSetLayout();
	vkAllocateDescriptorSets(vulkanDevice.GetVkDevice(), &allocInfo, &m_VkDescriptorSet);

}
