#include "Buffers/BufferBlock.h"

USING_NAMESPACE(Spectre)

void BufferBlock::Alloc(uint32_t size)
{
	m_UseSize = size;
	m_StageBufferPtr = VulkanBuffer::Create(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void BufferBlock::PushBuffer(void* pBuffer, uint32_t size)
{
	m_StageBufferPtr->Map(pBuffer, size);
}

void BufferBlock::Upload(VkBufferUsageFlagBits usage)
{
	m_DeviceBufferPtr = VulkanBuffer::Create(m_UseSize, usage,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	m_StageBufferPtr->CopyTo(m_DeviceBufferPtr);
}

