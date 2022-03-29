#include "VulkanDescriptor.h"


USING_NAMESPACE(Spectre)

DescriptorAllocator* DescriptorAllocator::Instance = nullptr;

void DescriptorAllocator::Allocate(VkDescriptorSet* set, VkDescriptorSetLayout layout)
{
	if (m_CurrentPool == VK_NULL_HANDLE)
	{
		m_CurrentPool = CreatePool();
		m_UsedPools.push_back(m_CurrentPool);
	}

	VkDevice device = VulkanEngine::GetInstance()->GetVkDevice();
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_CurrentPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &layout;
	VkResult ret = vkAllocateDescriptorSets(device, &allocInfo, set);

	if (ret == VK_ERROR_FRAGMENTED_POOL || ret == VK_ERROR_OUT_OF_POOL_MEMORY)
	{
		m_CurrentPool = CreatePool();
		m_UsedPools.push_back(m_CurrentPool);

		vkAllocateDescriptorSets(device, &allocInfo, set);
	}
}

VkDescriptorPool DescriptorAllocator::CreatePool()
{
	std::vector<VkDescriptorPoolSize> sizes;
	sizes.reserve(PoolSizes.size());
	for (auto sz : PoolSizes) {
		sizes.push_back({ sz.first, uint32_t(sz.second * MaxSets) });
	}
	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.maxSets = MaxSets;
	poolInfo.poolSizeCount = (uint32_t)sizes.size();
	poolInfo.pPoolSizes = sizes.data();

	VkDescriptorPool descriptorPool;
	vkCreateDescriptorPool(VulkanEngine::GetInstance()->GetVkDevice(), &poolInfo, nullptr, &descriptorPool);

	return descriptorPool;
}



void DescriptorLayoutCache::Cleanup()
{
	for (auto pair : m_LayoutCache)
	{
		vkDestroyDescriptorSetLayout(VulkanEngine::GetInstance()->GetVkDevice(), pair.second, nullptr);
	}
}

VkDescriptorSetLayout DescriptorLayoutCache::CreateDescriptorLayout(VkDescriptorSetLayoutCreateInfo* info)
{
	DescriptorLayoutInfo layoutinfo;
	layoutinfo.Bindings.reserve(info->bindingCount);
	bool isSorted = true;
	int32_t lastBinding = -1;
	for (uint32_t i = 0; i < info->bindingCount; i++) {
		layoutinfo.Bindings.push_back(info->pBindings[i]);

		//check that the bindings are in strict increasing order
		if (static_cast<int32_t>(info->pBindings[i].binding) > lastBinding)
		{
			lastBinding = info->pBindings[i].binding;
		}
		else {
			isSorted = false;
		}
	}
	if (!isSorted)
	{
		std::sort(layoutinfo.Bindings.begin(), layoutinfo.Bindings.end(), [](VkDescriptorSetLayoutBinding& a, VkDescriptorSetLayoutBinding& b) {
			return a.binding < b.binding;
		});
	}

	auto it = m_LayoutCache.find(layoutinfo);
	if (it != m_LayoutCache.end())
	{
		return (*it).second;
	}
	else {
		VkDescriptorSetLayout layout;
		vkCreateDescriptorSetLayout(VulkanEngine::GetInstance()->GetVkDevice(), info, nullptr, &layout);

		m_LayoutCache[layoutinfo] = layout;
		return layout;
	}
}

DescriptorLayoutCache* DescriptorLayoutCache::Instance = nullptr;

bool DescriptorLayoutCache::DescriptorLayoutInfo::operator==(const DescriptorLayoutInfo& other) const
{
	if (other.Bindings.size() != Bindings.size())
	{
		return false;
	}
	else {
		//compare each of the bindings is the same. Bindings are sorted so they will match
		for (int i = 0; i < Bindings.size(); i++) {
			if (other.Bindings[i].binding != Bindings[i].binding)
			{
				return false;
			}
			if (other.Bindings[i].descriptorType != Bindings[i].descriptorType)
			{
				return false;
			}
			if (other.Bindings[i].descriptorCount != Bindings[i].descriptorCount)
			{
				return false;
			}
			if (other.Bindings[i].stageFlags != Bindings[i].stageFlags)
			{
				return false;
			}
		}
		return true;
	}
}

size_t DescriptorLayoutCache::DescriptorLayoutInfo::Hash() const
{
	using std::size_t;
	using std::hash;

	size_t result = hash<size_t>()(Bindings.size());

	for (const VkDescriptorSetLayoutBinding& b : Bindings)
	{
		//pack the binding data into a single int64. Not fully correct but its ok
		size_t binding_hash = b.binding | b.descriptorType << 8 | b.descriptorCount << 16 | b.stageFlags << 24;

		//shuffle the packed binding data and xor it with the main hash
		result ^= hash<size_t>()(binding_hash);
	}

	return result;
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


VkDescriptorSet VulkanDescriptorBuilder::Build(VkDescriptorBufferInfo* bufferInfo)
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
	DescriptorAllocator::GetInstance()->Allocate(&set, m_Layout);

	for (VkWriteDescriptorSet& w : writes) {
		w.dstSet = set;
	}

	vkUpdateDescriptorSets(VulkanEngine::GetInstance()->GetVkDevice(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);

	return set;
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

	m_Layout = DescriptorLayoutCache::GetInstance()->CreateDescriptorLayout(&layoutInfo);

	return m_Layout;
}
