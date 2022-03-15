#include "Buffers/BufferBlock.h"

USING_NAMESPACE(Spectre)

void BufferBlock::Alloc(uint32_t size)
{
	m_StageBufferPtr = VulkanBuffer::Create(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

