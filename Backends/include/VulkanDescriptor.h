#pragma once
#include "VulkanEngine.h"

BEGIN_NAMESPACE_SPECTRE

class DescriptorAllocator
{
private:
	const MaxSets = 1000;

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

	
		


};

class VulkanDescriptor
{
public:

};

END_NAMESPACE_SPECTRE
