#include "VulkanCommon.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"

USING_NAMESPACE(Spectre)

VulkanBuffer::VulkanBuffer(const std::shared_ptr<const VulkanDevice>& vulkanDevice):
	m_DevicePtr(vulkanDevice)
{

}

VulkanBuffer::~VulkanBuffer()
{
	Release();
}

void VulkanBuffer::CreateHostBuffer(const void* ptr, uint32_t size)
{
	CreateBuffer(ptr, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	m_BufferType = Buffer_Type_Host;
}


void VulkanBuffer::CreateHostUniformBuffer(const void* ptr, uint32_t size)
{
	CreateBuffer(ptr, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	m_BufferType = Buffer_Type_Host;
}

void VulkanBuffer::CreateDeviceVertexBuffer(uint32_t size)
{
	CreateBuffer(nullptr, size, VkBufferUsageFlagBits(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT),
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	m_BufferType = Buffer_Type_Device;
}

void VulkanBuffer::CreateDeviceIndexBuffer(uint32_t size)
{
	CreateBuffer(nullptr, size, VkBufferUsageFlagBits(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT),
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	m_BufferType = Buffer_Type_Device;
}

void VulkanBuffer::MapToDevice(VulkanBuffer& dstBuffer, const VkCommandPool& commandPool, const VkCommandBuffer& commandBuffer)
{
	EXP_CHECK(dstBuffer.m_BufferType == Buffer_Type_Device, "dstBuffer type is not Buffer_Type_Device!");
	VkDevice device = m_DevicePtr->GetVkDevice();

	VkBufferCopy copyRegion = {};
	copyRegion.size = m_Size;
	vkCmdCopyBuffer(commandBuffer, m_VkbBuffer, dstBuffer.m_VkbBuffer, 1, &copyRegion);
}

void VulkanBuffer::UpdateHostBuffer(const void* ptr)
{
	VkDevice device = m_DevicePtr->GetVkDevice();
	void* dataPtr = nullptr;
	vkMapMemory(device, m_VkMemory, 0, m_Size, 0, &dataPtr);
	std::memcpy(dataPtr, ptr, m_Size);
	vkUnmapMemory(device, m_VkMemory);
}

void VulkanBuffer::Release()
{
	VkDevice device = m_DevicePtr->GetVkDevice();

	vkDestroyBuffer(device, m_VkbBuffer, nullptr);
	m_VkbBuffer = VK_NULL_HANDLE;



	vkFreeMemory(device, m_VkMemory, nullptr);
	m_VkMemory = VK_NULL_HANDLE;

	m_Size = 0;
}

void VulkanBuffer::CreateBuffer(const void* ptr, uint32_t size, VkBufferUsageFlagBits usage, VkMemoryPropertyFlags memoryFlags)
{
	m_Size = size;
	VkDevice device = m_DevicePtr->GetVkDevice();

	VkBufferCreateInfo bufferCI{};
	bufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCI.size = m_Size;
	bufferCI.usage = usage;
	vkCreateBuffer(device, &bufferCI, nullptr, &m_VkbBuffer);

	VkMemoryRequirements memReqInfo;
	VkMemoryAllocateInfo memAllocInfo{};
	memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

	vkGetBufferMemoryRequirements(device, m_VkbBuffer, &memReqInfo);
	uint32_t memoryTypeIndex = m_DevicePtr->GetPhysicalDevice().GetMemoryTypeIndex(memReqInfo.memoryTypeBits, memoryFlags);
	memAllocInfo.allocationSize = memReqInfo.size;
	memAllocInfo.memoryTypeIndex = memoryTypeIndex;
	vkAllocateMemory(device, &memAllocInfo, nullptr, &m_VkMemory);
	vkBindBufferMemory(device, m_VkbBuffer, m_VkMemory, 0);

	if (ptr!=nullptr)
	{
		void* dataPtr = nullptr;
		vkMapMemory(device, m_VkMemory, 0, memAllocInfo.allocationSize, 0, &dataPtr);
		std::memcpy(dataPtr, ptr, size);
		vkUnmapMemory(device, m_VkMemory);
	}
}
