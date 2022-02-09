#include "VulkanCommon.h"
#include "VulkanDevice.h"
#include "VulkanDescriptorSetLayout.h"

USING_NAMESPACE(Spectre)

std::shared_ptr<VulkanDescriptorSetLayout> VulkanDescriptorSetLayout::CreateDescriptorSetLayout(const VulkanDevice& vulkanDevice)
{
	auto* pDescriptorSetLayout = new VulkanDescriptorSetLayout(vulkanDevice);
	return std::shared_ptr<VulkanDescriptorSetLayout>(pDescriptorSetLayout);
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
	Destory();
}

void VulkanDescriptorSetLayout::Destory()
{
	vkDestroyDescriptorSetLayout(m_Device.GetVkDevice(), m_VkDescriptorSetLayout, nullptr);
}

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const VulkanDevice& vulkanDevice):
	m_Device(vulkanDevice)
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
	vkCreateDescriptorSetLayout(m_Device.GetVkDevice(), &descSetLayoutInfo, nullptr, &m_VkDescriptorSetLayout);
}
