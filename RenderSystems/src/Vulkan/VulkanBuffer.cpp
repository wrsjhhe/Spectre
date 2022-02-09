#include "VulkanCommon.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"

USING_NAMESPACE(Spectre)

VulkanBuffer::VulkanBuffer(const VulkanDevice& vulkanDevice, BufferType bufferType):
	m_Device(vulkanDevice),
	m_BufferType(bufferType)
{

}

VulkanBuffer::~VulkanBuffer()
{
	Release();
}

std::shared_ptr<VulkanBuffer> VulkanBuffer::CreateHostBuffer(const VulkanDevice& vulkanDevice, const void* ptr, uint32_t size)
{
	auto* pBuffer = new VulkanBuffer(vulkanDevice, Buffer_Type_Host);

	pBuffer->CreateBuffer(ptr, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	return std::shared_ptr<VulkanBuffer>{pBuffer};
}


std::shared_ptr<VulkanBuffer> VulkanBuffer::CreateHostUniformBuffer(const VulkanDevice& vulkanDevice, const void* ptr, uint32_t size)
{
	auto* pBuffer = new VulkanBuffer(vulkanDevice, Buffer_Type_Host);

	pBuffer->CreateBuffer(ptr, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	return std::shared_ptr<VulkanBuffer>{pBuffer};
}

std::shared_ptr<VulkanBuffer> VulkanBuffer::CreateDeviceVertexBuffer(const VulkanDevice& vulkanDevice, uint32_t size)
{
	auto* pBuffer = new VulkanBuffer(vulkanDevice, Buffer_Type_Device);
	pBuffer->CreateBuffer(nullptr, size, VkBufferUsageFlagBits(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT),
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	return std::shared_ptr<VulkanBuffer>{pBuffer};
}

std::shared_ptr<VulkanBuffer> VulkanBuffer::CreateDeviceIndexBuffer(const VulkanDevice& vulkanDevice, uint32_t size)
{
	auto* pBuffer = new VulkanBuffer(vulkanDevice, Buffer_Type_Device);
	pBuffer->CreateBuffer(nullptr, size, VkBufferUsageFlagBits(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT),
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	return std::shared_ptr<VulkanBuffer>{pBuffer};
}

void VulkanBuffer::MapToDevice(VulkanBuffer& dstBuffer, const VkCommandPool& commandPool, const VkCommandBuffer& commandBuffer)
{
	EXP_CHECK(dstBuffer.m_BufferType == Buffer_Type_Device, "dstBuffer type is not Buffer_Type_Device!");
	VkDevice device = m_Device.GetVkDevice();

	VkBufferCopy copyRegion = {};
	copyRegion.size = m_Size;
	vkCmdCopyBuffer(commandBuffer, m_VkbBuffer, dstBuffer.m_VkbBuffer, 1, &copyRegion);
}

void VulkanBuffer::UpdateHostBuffer(const void* ptr)
{
	VkDevice device = m_Device.GetVkDevice();
	void* dataPtr = nullptr;
	vkMapMemory(device, m_VkMemory, 0, m_Size, 0, &dataPtr);
	std::memcpy(dataPtr, ptr, m_Size);
	vkUnmapMemory(device, m_VkMemory);
}

void VulkanBuffer::Release()
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
		void* dataPtr = nullptr;
		vkMapMemory(device, m_VkMemory, 0, memAllocInfo.allocationSize, 0, &dataPtr);
		std::memcpy(dataPtr, ptr, size);
		vkUnmapMemory(device, m_VkMemory);
	}
}
