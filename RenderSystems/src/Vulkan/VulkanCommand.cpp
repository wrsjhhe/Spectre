#include "VulkanCommon.h"
#include "VulkanDevice.h"
#include "VulkanQueue.h"
#include "VulkanCommand.h"

USING_NAMESPACE(Spectre)

std::vector<std::shared_ptr<VulkanCommand>> VulkanCommand::Create(const VulkanDevice& vulkanDevice,
	const VkCommandPool& commandPool, uint32_t size)
{
	std::vector<std::shared_ptr<VulkanCommand>> buffers;

	std::vector<VkCommandBuffer> vkBuffers(size);
	VkCommandBufferAllocateInfo cmdBufferInfo{};
	cmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdBufferInfo.commandBufferCount = size;
	cmdBufferInfo.commandPool = commandPool;

	vkAllocateCommandBuffers(vulkanDevice.GetVkDevice(), &cmdBufferInfo, vkBuffers.data());

	for (uint32_t i = 0; i < size; ++i)
	{
		auto* commandBuffers = new VulkanCommand(vulkanDevice, commandPool, vkBuffers.at(i));
		buffers.emplace_back(commandBuffers);
	}


	return buffers;
}


VulkanCommand::~VulkanCommand()
{
	Free();
}


void VulkanCommand::RecordCommond(std::function<void(VkCommandBuffer)> recordCmd)
{
	recordCmd(m_VkCommandBuffer);
}

void VulkanCommand::Submit(VulkanQueue& queue)
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

VulkanCommand::VulkanCommand(const VulkanDevice& vulkanDevice,
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

void VulkanCommand::Free()
{
	vkFreeCommandBuffers(m_Device.GetVkDevice(), m_CommandPool, 1, &m_VkCommandBuffer);
	vkDestroyFence(m_Device.GetVkDevice(), m_VkFence, nullptr);
}
