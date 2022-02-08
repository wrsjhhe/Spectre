#include "VulkanCommon.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"

USING_NAMESPACE(Spectre)

VulkanBuffer::VulkanBuffer(const std::shared_ptr<VulkanDevice>& vulkanDevice):
	m_VulkanDevicePtr(vulkanDevice)
{

}

void VulkanBuffer::CreateHostBuffer(void* ptr, uint32_t size)
{
	CreateBuffer(ptr, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void VulkanBuffer::CreateDeviceVertexBuffer(uint32_t size)
{
	CreateBuffer(nullptr, size, VkBufferUsageFlagBits(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT),
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

void VulkanBuffer::CreateDeviceIndexBuffer(uint32_t size)
{
	CreateBuffer(nullptr, size, VkBufferUsageFlagBits(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT),
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

void VulkanBuffer::MapToDevice(const VulkanBuffer& dstBuffer, const VkCommandBuffer& commandBuffer)
{
	VkDevice device = m_VulkanDevicePtr->GetVkDevice();

	VkCommandBuffer xferCmdBuffer;
	// gfx queue自带transfer功能，为了优化需要使用专有的xfer queue。这里为了简单，先将就用。
	VkCommandBufferAllocateInfo xferCmdBufferInfo{};
	xferCmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	xferCmdBufferInfo.commandPool = commandPool;
	xferCmdBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	xferCmdBufferInfo.commandBufferCount = 1;
	vkAllocateCommandBuffers(device, &xferCmdBufferInfo, &xferCmdBuffer);

	// 开始录制命令
	VkCommandBufferBeginInfo cmdBufferBeginInfo{};
	cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkBeginCommandBuffer(xferCmdBuffer, &cmdBufferBeginInfo);

	VkBufferCopy copyRegion = {};
	copyRegion.size = vertices.size() * sizeof(Vertex);
	vkCmdCopyBuffer(xferCmdBuffer, tempVertexBuffer.buffer, m_VertexBuffer.buffer, 1, &copyRegion);

	// 结束录制
	vkEndCommandBuffer(xferCmdBuffer);
}

void VulkanBuffer::CreateBuffer(void* ptr, uint32_t size, VkBufferUsageFlagBits usage, VkMemoryPropertyFlags memoryFlags)
{
	VkDevice device = m_VulkanDevicePtr->GetVkDevice();

	VkBufferCreateInfo bufferCI{};
	bufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCI.size = size;
	bufferCI.usage = usage;
	vkCreateBuffer(device, &bufferCI, nullptr, &m_VkbBuffer);

	VkMemoryRequirements memReqInfo;
	VkMemoryAllocateInfo memAllocInfo{};
	memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

	vkGetBufferMemoryRequirements(device, m_VkbBuffer, &memReqInfo);
	uint32_t memoryTypeIndex = m_VulkanDevicePtr->GetPhysicalDevice().GetMemoryTypeIndex(memReqInfo.memoryTypeBits, memoryFlags);
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
