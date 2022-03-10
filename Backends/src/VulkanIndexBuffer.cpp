#include "VulkanCommon.h"
#include "VulkanBuffer.h"
#include "VulkanIndexBuffer.h"

USING_NAMESPACE(Spectre)



VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, uint32_t count, VkIndexType indexType):
	m_IndexType(indexType)
{
	uint32_t bufferSize = count * sizeof(uint32_t);
	m_HostBuffer = VulkanBuffer::Create(bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indices);

	m_DeviceBuffer = VulkanBuffer::Create(bufferSize,
		VkBufferUsageFlagBits(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

VulkanIndexBuffer::VulkanIndexBuffer(uint16_t* indices, uint32_t count, VkIndexType indexType):
	m_IndexType(indexType)
{
	uint32_t bufferSize = count * sizeof(uint16_t);
	m_HostBuffer = VulkanBuffer::Create(bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indices);
}

void VulkanIndexBuffer::Synchronize(const VkCommandBuffer& commandBuffer)
{
	m_HostBuffer->CopyTo(*m_DeviceBuffer, commandBuffer);
}

void VulkanIndexBuffer::CmdBind(const VkCommandBuffer& commandBuffer)
{
	vkCmdBindIndexBuffer(commandBuffer, m_DeviceBuffer->GetVkBuffer(), 0, m_IndexType);
}