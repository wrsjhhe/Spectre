#include "VulkanCommon.h"
#include "VulkanDevice.h"
#include "RenderDefs.h"
#include "VulkanBuffer.h"

USING_NAMESPACE(Spectre)

std::shared_ptr<VulkanBuffer> VulkanBuffer::Create(const VulkanDevice& vulkanDevice, uint32_t size,
	VkBufferUsageFlagBits usage, VkMemoryPropertyFlags memoryFlags,const void* data)
{
	auto* pBuffer = new VulkanBuffer(vulkanDevice);

	pBuffer->CreateBuffer(data, size, usage, memoryFlags);

	return std::shared_ptr<VulkanBuffer>{pBuffer};
}

VulkanBuffer::VulkanBuffer(const VulkanDevice& vulkanDevice):
	m_Device(vulkanDevice)
{

}

VulkanBuffer::~VulkanBuffer()
{
	Destroy();
}


void VulkanBuffer::CopyTo(VulkanBuffer& dstBuffer ,const VkCommandBuffer& commandBuffer)
{
	VkDevice device = m_Device.GetVkDevice();

	VkBufferCopy copyRegion = {};
	copyRegion.size = m_Size;
	vkCmdCopyBuffer(commandBuffer, m_VkbBuffer, dstBuffer.m_VkbBuffer, 1, &copyRegion);
}

VkResult VulkanBuffer::Map(VkDeviceSize size, VkDeviceSize offset)
{
	if (m_DataPtr)
	{
		return VK_ERROR_MEMORY_MAP_FAILED;
	}

	return vkMapMemory(m_Device.GetVkDevice(), m_VkMemory, offset, size, 0, &m_DataPtr);
}

void VulkanBuffer::UnMap()
{
	if (!m_DataPtr)
	{
		return;
	}
	vkUnmapMemory(m_Device.GetVkDevice(), m_VkMemory);
	m_DataPtr = nullptr;
}

void VulkanBuffer::UpdateHostBuffer(const void* ptr)
{
	VkDevice device = m_Device.GetVkDevice();
	void* dataPtr = nullptr;
	vkMapMemory(device, m_VkMemory, 0, m_Size, 0, &dataPtr);
	std::memcpy(dataPtr, ptr, m_Size);
	vkUnmapMemory(device, m_VkMemory);
}

void VulkanBuffer::Destroy()
{
	VkDevice device = m_Device.GetVkDevice();

	vkDestroyBuffer(device, m_VkbBuffer, nullptr);
	m_VkbBuffer = VK_NULL_HANDLE;

	vkFreeMemory(device, m_VkMemory, nullptr);
	m_VkMemory = VK_NULL_HANDLE;

	m_Size = 0;
}

void VulkanBuffer::CreateBuffer(const void* ptr, uint32_t size, VkBufferUsageFlagBits usage, VkMemoryPropertyFlags memoryFlags)
{
	m_Size = size;
	VkDevice device = m_Device.GetVkDevice();

	VkBufferCreateInfo bufferCI{};
	bufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCI.size = m_Size;
	bufferCI.usage = usage;
	vkCreateBuffer(device, &bufferCI, nullptr, &m_VkbBuffer);

	VkMemoryRequirements memReqInfo;
	VkMemoryAllocateInfo memAllocInfo{};
	memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

	vkGetBufferMemoryRequirements(device, m_VkbBuffer, &memReqInfo);
	uint32_t memoryTypeIndex = m_Device.GetPhysicalDevice().GetMemoryTypeIndex(memReqInfo.memoryTypeBits, memoryFlags);
	memAllocInfo.allocationSize = memReqInfo.size;
	memAllocInfo.memoryTypeIndex = memoryTypeIndex;
	vkAllocateMemory(device, &memAllocInfo, nullptr, &m_VkMemory);
	vkBindBufferMemory(device, m_VkbBuffer, m_VkMemory, 0);

	if (ptr!=nullptr)
	{
		Map();
		std::memcpy(m_DataPtr, ptr, size);
		if ((memoryFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
		{
			Flush();
		}
		UnMap();
	}
}

VkResult VulkanBuffer::Flush(VkDeviceSize size, VkDeviceSize offset)
{
	VkMappedMemoryRange mappedRange = {};
	mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.memory = m_VkMemory;
	mappedRange.offset = offset;
	mappedRange.size = size;
	return vkFlushMappedMemoryRanges(m_Device.GetVkDevice(), 1, &mappedRange);
}