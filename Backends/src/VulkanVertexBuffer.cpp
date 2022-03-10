#include "VulkanCommon.h"
#include "VulkanBuffer.h"
#include "VulkanVertexBuffer.h"

USING_NAMESPACE(Spectre)


std::shared_ptr<VulkanVertexBuffer> VulkanVertexBuffer::Create(float* vertices,uint32_t count)
{
	auto* pBuffer = new VulkanVertexBuffer(vertices,count);
	return std::shared_ptr<VulkanVertexBuffer>(pBuffer);
}

VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t count)
{
	uint32_t bufferSize = count * sizeof(float);
	m_HostBuffer = VulkanBuffer::Create(bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertices);

	m_DeviceBuffer = VulkanBuffer::Create(bufferSize,
		VkBufferUsageFlagBits(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


}


void VulkanVertexBuffer::Synchronize(const VkCommandBuffer& commandBuffer)
{
	m_HostBuffer->CopyTo(*m_DeviceBuffer, commandBuffer);
}

void VulkanVertexBuffer::CmdBind(const VkCommandBuffer& commandBuffer, const VkDeviceSize* pOffset)
{
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_DeviceBuffer->GetVkBuffer(), pOffset);
}

