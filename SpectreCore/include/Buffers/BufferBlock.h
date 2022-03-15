#pragma once
#include "SpectreApi.h"
#include "VulkanBuffer.h"
BEGIN_NAMESPACE_SPECTRE

class BufferBlock
{
public:
	void Alloc(uint32_t size);
private:
	VulkanBufferPtr m_StageBufferPtr;
};

END_NAMESPACE_SPECTRE