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
	vkCreateBuffer(device, &bufferCI, nullptr, &m_VkBuffer);

	VkMemoryRequirements memReqInfo;
	VkMemoryAllocateInfo memAllocInfo{};
	memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

	vkGetBufferMemoryRequirements(device, m_VkBuffer, &memReqInfo);
	uint32_t memoryTypeIndex = VulkanEngine::GetInstance()->GetMemoryTypeIndex(memReqInfo.memoryTypeBits, (VkMemoryPropertyFlags)memoryFlags);
	memAllocInfo.allocationSize = memReqInfo.size;
	memAllocInfo.memoryTypeIndex = memoryTypeIndex;
	vkAllocateMemory(device, &memAllocInfo, nullptr, &m_VkMemory);
	vkBindBufferMemory(device, m_VkBuffer, m_VkMemory, 0);
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
	cmd->RecordCommond([&](VkCommandBuffer cmdBuffer) {
		vkCmdCopyBuffer(cmdBuffer, m_VkBuffer, dstBuffer->m_VkBuffer, 1, &copyRegion);
	});

	cmd->Submit(VulkanEngine::GetInstance()->GetGraphicQueue());
}

void VulkanBuffer::CopyToImage(VulkanImagePtr dstImage,uint32_t width, uint32_t height)
{
	auto cmd = VulkanEngine::GetInstance()->GetTransformCmd();

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};
	cmd->RecordCommond([&](VkCommandBuffer cmdBuffer) {
		vkCmdCopyBufferToImage(cmdBuffer, m_VkBuffer, dstImage->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	});
}

void VulkanBuffer::Map(void* ptr, uint32_t size, uint32_t offset)
{
	uint32_t mapSize = size;
	if (mapSize == 0)
	{
		mapSize = m_TotalSize;
	}
	MapPointerCache = nullptr;
	VkDevice device = VulkanEngine::GetInstance()->GetVkDevice();
	vkMapMemory(device, m_VkMemory, offset, mapSize, 0, &MapPointerCache);
	std::memcpy(MapPointerCache, ptr, mapSize);
}

void VulkanBuffer::UnMap()
{
	vkUnmapMemory(VulkanEngine::GetInstance()->GetVkDevice(), m_VkMemory);
	MapPointerCache = nullptr;
}


void VulkanBuffer::Update(void* ptr, uint32_t size /*= 0*/, uint32_t offset /*= 0*/)
{
	if (MapPointerCache == nullptr)
	{
		Map(ptr, size, offset);
	}
	else
	{
		uint32_t mapSize = size;
		if (mapSize == 0)
		{
			mapSize = m_TotalSize;
		}
		std::memcpy((char*)MapPointerCache+ offset, ptr, mapSize);
	}
}

void VulkanBuffer::Destroy()
{
	if (MapPointerCache)
	{
		UnMap();
	}

	VkDevice device = VulkanEngine::GetInstance()->GetVkDevice();

	vkDestroyBuffer(device, m_VkBuffer, nullptr);
	m_VkBuffer = VK_NULL_HANDLE;

	vkFreeMemory(device, m_VkMemory, nullptr);
	m_VkMemory = VK_NULL_HANDLE;

	m_TotalSize = 0;
}


void VulkanBuffer::Flush(uint64_t size, uint64_t offset)
{
	VkMappedMemoryRange mappedRange = {};
	mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.memory = m_VkMemory;
	mappedRange.size = size;
	mappedRange.offset = offset;

	vkFlushMappedMemoryRanges(VulkanEngine::GetInstance()->GetVkDevice(), 1, &mappedRange);
}