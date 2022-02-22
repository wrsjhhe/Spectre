#include "VulkanCommon.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"
#include "VulkanIndexBuffer.h"

USING_NAMESPACE(Spectre)



VulkanIndexBuffer::VulkanIndexBuffer(const VulkanDevice& vulkanDevice, std::vector<uint32_t> indices, VkIndexType indexType):
	m_IndexCount(indices.size()),
	m_IndexType(indexType)
{
	uint32_t bufferSize = indices.size() * sizeof(uint32_t);
	m_HostBuffer = VulkanBuffer::Create(vulkanDevice, bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indices.data());

	m_DeviceBuffer = VulkanBuffer::Create(vulkanDevice, bufferSize,
		VkBufferUsageFlagBits(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

VulkanIndexBuffer::VulkanIndexBuffer(const VulkanDevice& vulkanDevice, std::vector<uint16_t> indices, VkIndexType indexType):
	m_IndexCount(indices.size()),
	m_IndexType(indexType)
{
	uint32_t bufferSize = indices.size() * sizeof(uint16_t);
	m_HostBuffer = VulkanBuffer::Create(vulkanDevice, indices.size() * sizeof(uint16_t),
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indices.data());
}

void VulkanIndexBuffer::Synchronize(const VkCommandBuffer& commandBuffer)
{
	m_HostBuffer->CopyTo(*m_DeviceBuffer, commandBuffer);
}

void VulkanIndexBuffer::CmdBind(const VkCommandBuffer& commandBuffer)
{
	vkCmdBindIndexBuffer(commandBuffer, m_DeviceBuffer->GetVkBuffer(), 0, m_IndexType);
}