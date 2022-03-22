#include "Buffers/BufferBlock.h"
#include "Allocator.h"
USING_NAMESPACE(Spectre)
static std::map<int, uint32_t> s_BufferLevelSizeMap{ 
	{0, 1024 * 1024 * 1}, 
	{1, 1024 * 1024 * 5}, 
	{2, 1024 * 1024 * 20}, 
	{3, 1024 * 1024 * 60}, 
	{4, 1024 * 1024 * 120}, 
	{5, 1024 * 1024 * 240}, 
	{6, 1024 * 1024 * 480}, 
};

BufferBlock::BufferBlock()
{
}

void BufferBlock::Alloc(uint32_t size)
{
	if (m_DataPtr != nullptr)
		Free();
	m_DataPtr = Spectre::heap_alloc(size);

	m_UsedSize = size;
	m_Capacity = size;

	auto iter = s_BufferLevelSizeMap.begin();
	while (iter!=s_BufferLevelSizeMap.end())
	{
		if (iter->second > size)
		{
			m_CurrentSizeLevel = iter->first;
			break;
		}
	}
	if (iter == s_BufferLevelSizeMap.end())
	{
		m_CurrentSizeLevel = -1;
	}

	//m_StageBufferPtr = VulkanBuffer::Create(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	//	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void BufferBlock::Relloc(uint32_t newSize)
{
	if (m_DataPtr == nullptr)
	{
		Alloc(newSize);
		return;
	}

	if (newSize > m_UsedSize)
	{
		if (newSize <= m_Capacity)
			return;

		

	}
}

void BufferBlock::PushBuffer(void* pBuffer, uint32_t size)
{
	uint32_t newSize = m_UsedSize + size;
	Relloc(newSize);
	//m_StageBufferPtr->Map(pBuffer, newSize,0);
}

void BufferBlock::Upload(VkBufferUsageFlagBits usage)
{
	m_DeviceBufferPtr = VulkanBuffer::Create(m_UsedSize, usage,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	//m_StageBufferPtr->CopyTo(m_DeviceBufferPtr);
}

void BufferBlock::Free()
{
	Spectre::aligned_free(m_DataPtr);
	m_DataPtr = nullptr;
	m_UsedSize = 0;
	m_Capacity = 0;
}

