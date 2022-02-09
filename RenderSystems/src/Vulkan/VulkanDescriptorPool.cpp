#include "VulkanCommon.h"
#include "VulkanDevice.h"
#include "VulkanDescriptorPool.h"

USING_NAMESPACE(Spectre)

std::shared_ptr<VulkanDescriptorPool> VulkanDescriptorPool::CreateDescriptorPool(const VulkanDevice& vulkanDevice)
{
	auto* pDescriptorPool = new VulkanDescriptorPool(vulkanDevice);
	return std::shared_ptr<VulkanDescriptorPool>(pDescriptorPool);
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
	Destory();
}

void VulkanDescriptorPool::Destory()
{
	vkDestroyDescriptorPool(m_Device.GetVkDevice(), m_VkDescriptorPool, nullptr);
}

VulkanDescriptorPool::VulkanDescriptorPool(const VulkanDevice& vulkanDevice):
	m_Device(vulkanDevice)
{
	VkDescriptorPoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = 1;

	VkDescriptorPoolCreateInfo descriptorPoolInfo{};
	descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.poolSizeCount = 1;
	descriptorPoolInfo.pPoolSizes = &poolSize;
	descriptorPoolInfo.maxSets = 1;
	vkCreateDescriptorPool(m_Device.GetVkDevice(), &descriptorPoolInfo, nullptr, &m_VkDescriptorPool);
}
