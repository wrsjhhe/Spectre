#include "VulkanCommon.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanQueue.h"
#include "VulkanCommandBuffer.h"

USING_NAMESPACE(Spectre)

std::vector<std::shared_ptr<VulkanCommandBuffer>> VulkanCommandBuffer::CreataGraphicBuffers(const VulkanDevice& vulkanDevice,
	const VulkanCommandPool& commandPool, uint32_t size)
{
	std::vector<std::shared_ptr<VulkanCommandBuffer>> buffers;

	std::vector<VkCommandBuffer> vkBuffers(size);
	VkCommandBufferAllocateInfo cmdBufferInfo{};
	cmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdBufferInfo.commandBufferCount = size;
	cmdBufferInfo.commandPool = commandPool.GetVkGraphicCommandPool();

	vkAllocateCommandBuffers(vulkanDevice.GetVkDevice(), &cmdBufferInfo, vkBuffers.data());

	for (uint32_t i = 0; i < size; ++i)
	{
		auto* commandBuffers = new VulkanCommandBuffer(vulkanDevice, commandPool.GetVkGraphicCommandPool(), vkBuffers.at(i));
		buffers.emplace_back(commandBuffers);
	}


	return buffers;
}

//std::shared_ptr<VulkanCommandBuffers> VulkanCommandBuffers::CreataTransferBuffers(const VulkanDevice& vulkanDevice, 
//	const VulkanCommandPool& commandPool, uint32_t size)
//{
//	auto* commandBuffers = new VulkanCommandBuffers(vulkanDevice, commandPool.GetVkTransferCommandPool(), size);
//	return std::shared_ptr<VulkanCommandBuffers>(commandBuffers);
//}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
	Free();
}


void VulkanCommandBuffer::RecordCommond(std::function<void(VkCommandBuffer)> recordCmd)
{
	recordCmd(m_VkCommandBuffer);
}

void VulkanCommandBuffer::Submit(VulkanQueue& queue)
{
	if (!m_IsEnd)
	{
		m_IsEnd = true;
		vkEndCommandBuffer(m_VkCommandBuffer);
	}
	
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_VkCommandBuffer;
	submitInfo.signalSemaphoreCount = SignalSemaphore.size();
	submitInfo.pSignalSemaphores = SignalSemaphore.data();
	submitInfo.waitSemaphoreCount = WaitSemaphore.size();
	submitInfo.pWaitSemaphores = WaitSemaphore.data();
	submitInfo.pWaitDstStageMask = WaitStageMask.data();


	vkResetFences(m_Device.GetVkDevice(), 1, &m_VkFence);
	vkQueueSubmit(queue.GetVkQueue(), 1, &submitInfo, m_VkFence);
	vkWaitForFences(m_Device.GetVkDevice(), 1, &m_VkFence, true, 0x7fffffffffffffff);
}

VulkanCommandBuffer::VulkanCommandBuffer(const VulkanDevice& vulkanDevice,
	const VkCommandPool& commandPool, VkCommandBuffer buffer) :
	m_Device(vulkanDevice),
	m_CommandPool(commandPool),
	m_VkCommandBuffer(buffer)
{
	VkCommandBufferBeginInfo cmdBufferBeginInfo{};
	cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkBeginCommandBuffer(m_VkCommandBuffer, &cmdBufferBeginInfo);

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = 0;

	vkCreateFence(m_Device.GetVkDevice(), &fenceInfo, nullptr, &m_VkFence);
}

void VulkanCommandBuffer::Free()
{
	vkFreeCommandBuffers(m_Device.GetVkDevice(), m_CommandPool, 1, &m_VkCommandBuffer);
	vkDestroyFence(m_Device.GetVkDevice(), m_VkFence, nullptr);
}
