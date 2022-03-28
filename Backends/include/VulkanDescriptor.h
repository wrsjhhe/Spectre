#pragma once
#include "VulkanEngine.h"

BEGIN_NAMESPACE_SPECTRE

class DescriptorAllocator
{
public:
	static DescriptorAllocator* GetInstance()
	{
		if (Instance == nullptr)
		{
			Instance = new DescriptorAllocator();
		}
		return Instance;
	}

private:
	const uint32_t MaxSets = 1000;

    const std::vector<std::pair<VkDescriptorType, float>> PoolSizes =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.f },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.f }
	};

	struct PoolUseInfo
	{
		uint32_t Index = 0;

	};

public:
	void Allocate(VkDescriptorSet* set, VkDescriptorSetLayout layout);
	
private:
	VkDescriptorPool CreatePool();

private:
	static DescriptorAllocator* Instance;

	VkDescriptorPool m_CurrentPool = VK_NULL_HANDLE;
	std::vector<VkDescriptorPool> m_UsedPools;
	std::vector<VkDescriptorPool> m_FreePools;

};

class DescriptorLayoutCache 
{
public:
	static DescriptorLayoutCache* GetInstance()
	{
		if (Instance == nullptr)
		{
			Instance = new DescriptorLayoutCache();
		}
		return Instance;
	}

public:
	void Cleanup();

	VkDescriptorSetLayout CreateDescriptorLayout(VkDescriptorSetLayoutCreateInfo* info);

	struct DescriptorLayoutInfo {

		std::vector<VkDescriptorSetLayoutBinding> Bindings;

		bool operator==(const DescriptorLayoutInfo& other) const;

		size_t Hash() const;
	};



private:

	struct DescriptorLayoutHash
	{

		std::size_t operator()(const DescriptorLayoutInfo& k) const
		{
			return k.Hash();
		}
	};
	static DescriptorLayoutCache* Instance;
	std::unordered_map<DescriptorLayoutInfo, VkDescriptorSetLayout, DescriptorLayoutHash> m_LayoutCache;
};

class VulkanDescriptor;
typedef std::shared_ptr<VulkanDescriptor> VulkanDescriptorPtr;
class VulkanDescriptor
{
public:
	static VulkanDescriptorPtr Create();

	void BindBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo, 
		VkDescriptorType type, VkShaderStageFlags stageFlags);

	void Build();

	VkDescriptorSetLayout GetVkDescriptorSetLayout() const { return m_Layout; }

private:
	std::vector<VkDescriptorSetLayoutBinding> m_Bindings;
	std::vector<VkWriteDescriptorSet> m_Writes;
	VkDescriptorSet m_Set = VK_NULL_HANDLE;
	VkDescriptorSetLayout m_Layout = VK_NULL_HANDLE;
};

END_NAMESPACE_SPECTRE
