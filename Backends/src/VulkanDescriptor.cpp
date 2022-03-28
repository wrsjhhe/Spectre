#include "VulkanDescriptor.h"


USING_NAMESPACE(Spectre)


void DescriptorAllocator::Allocate()
{
	
}

VkDescriptorPool DescriptorAllocator::CreatePool()
{
	std::vector<VkDescriptorPoolSize> sizes;
	sizes.reserve(PoolSizes.size());
	for (auto sz : PoolSizes) {
		sizes.push_back({ sz.first, uint32_t(sz.second * MaxSets) });
	}
	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = MaxSets;
	pool_info.poolSizeCount = (uint32_t)sizes.size();
	pool_info.pPoolSizes = sizes.data();

	VkDescriptorPool descriptorPool;
	vkCreateDescriptorPool(VulkanEngine::GetInstance()->GetVkDevice(), &pool_info, nullptr, &descriptorPool);

	return descriptorPool;
}
