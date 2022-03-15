#include "VulkanCommon.h"
#include "VulkanEngine.h"
#include "RenderTypes.h"
#include "VulkanBuffer.h"

USING_NAMESPACE(Spectre)

std::shared_ptr<VulkanBuffer> VulkanBuffer::Create(uint32_t size, VkBufferUsageFlagBits usage, VkMemoryPropertyFlags memoryFlags)
{
	auto* pBuffer = new VulkanBuffer(size, usage, memoryFlags);

	return std::shared_ptr<VulkanBuffer>{pBuffer};
}

VulkanBuffer::VulkanBuffer(uint32_t size, VkBufferUsageFlagBits usage, VkMemoryPropertyFlags memoryFlags):
	m_TotalSize(size)
{
	VkDevice device = VulkanEngine::GetInstance()->GetVkDevice();

	VkBufferCreateInfo bufferCI{};
	bufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCI.size = m_TotalSize;
	bufferCI.usage = (VkBufferUsageFlags)usage;
	vkCreateBuffer(device, &bufferCI, nullptr, &m_VkbBuffer);

	VkMemoryRequirements memReqInfo;
	VkMemoryAllocateInfo memAllocInfo{};
	memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

	vkGetBufferMemoryRequirements(device, m_VkbBuffer, &memReqInfo);
	uint32_t memoryTypeIndex = VulkanEngine::GetInstance()->GetMemoryTypeIndex(memReqInfo.memoryTypeBits, (VkMemoryPropertyFlags)memoryFlags);
	memAllocInfo.allocationSize = memReqInfo.size;
	memAllocInfo.memoryTypeIndex = memoryTypeIndex;
	vkAllocateMemory(device, &memAllocInfo, nullptr, &m_VkMemory);
	vkBindBufferMemory(device, m_VkbBuffer, m_VkMemory, 0);
}

VulkanBuffer::~VulkanBuffer()
{
	Destroy();
}


void VulkanBuffer::CopyTo(VulkanBufferPtr dstBuffer)
{
	auto cmd = VulkanEngine::GetInstance()->GetTransformCmd();
	VkBufferCopy copyRegion = {};
	copyRegion.size = m_TotalSize;
	vkCmdCopyBuffer(cmd->GetVkCommandBuffer(), m_VkbBuffer, dstBuffer->m_VkbBuffer, 1, &copyRegion);
}

void VulkanBuffer::Map(void* ptr, uint32_t size, uint32_t offset, bool keepMap)
{
	uint32_t mapSize = size;
	MapPointerCache = nullptr;
	VkDevice device = VulkanEngine::GetInstance()->GetVkDevice();
	vkMapMemory(device, m_VkMemory, offset, size, 0, &MapPointerCache);
	std::memcpy(MapPointerCache, ptr, size);
	Flush();

	if (!keepMap)
		UnMap();
}

void VulkanBuffer::UnMap()
{
	vkUnmapMemory(VulkanEngine::GetInstance()->GetVkDevice(), m_VkMemory);
	MapPointerCache = nullptr;
}


void VulkanBuffer::Destroy()
{
	if (MapPointerCache)
	{
		UnMap();
	}

	VkDevice device = VulkanEngine::GetInstance()->GetVkDevice();

	vkDestroyBuffer(device, m_VkbBuffer, nullptr);
	m_VkbBuffer = VK_NULL_HANDLE;

	vkFreeMemory(device, m_VkMemory, nullptr);
	m_VkMemory = VK_NULL_HANDLE;

	m_TotalSize = 0;
}


void VulkanBuffer::Flush()
{
	VkMappedMemoryRange mappedRange = {};
	mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.memory = m_VkMemory;
	
	vkFlushMappedMemoryRanges(VulkanEngine::GetInstance()->GetVkDevice(), 1, &mappedRange);
}