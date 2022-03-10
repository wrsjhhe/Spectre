#include "VulkanCommon.h"
#include "VulkanEngine.h"
#include "VulkanCommand.h"

USING_NAMESPACE(Spectre)

std::vector<std::shared_ptr<VulkanCommand>> VulkanCommand::Create(const VkCommandPool& commandPool, uint32_t size)
{
	auto* pEngine = VulkanEngine::GetInstance();
	std::vector<std::shared_ptr<VulkanCommand>> buffers;

	std::vector<VkCommandBuffer> vkBuffers(size);
	VkCommandBufferAllocateInfo cmdBufferInfo{};
	cmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdBufferInfo.commandBufferCount = size;
	cmdBufferInfo.commandPool = commandPool;

	vkAllocateCommandBuffers(pEngine->GetVkDevice(), &cmdBufferInfo, vkBuffers.data());

	for (uint32_t i = 0; i < size; ++i)
	{
		auto* commandBuffers = new VulkanCommand(commandPool, vkBuffers.at(i));
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

	auto* pEngine = VulkanEngine::GetInstance();
	vkResetFences(pEngine->GetVkDevice(), 1, &m_VkFence);
	vkQueueSubmit(queue.VkQueue, 1, &submitInfo, m_VkFence);
	vkWaitForFences(pEngine->GetVkDevice(), 1, &m_VkFence, true, 0x7fffffffffffffff);
}

void VulkanCommand::Reset()
{
	vkResetCommandBuffer(m_VkCommandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
}

VulkanCommand::VulkanCommand(const VkCommandPool& commandPool, VkCommandBuffer buffer) :
	m_CommandPool(commandPool),
	m_VkCommandBuffer(buffer)
{
	auto* pEngine = VulkanEngine::GetInstance();
	VkCommandBufferBeginInfo cmdBufferBeginInfo{};
	cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkBeginCommandBuffer(m_VkCommandBuffer, &cmdBufferBeginInfo);

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = 0;

	vkCreateFence(pEngine->GetVkDevice(), &fenceInfo, nullptr, &m_VkFence);
}

void VulkanCommand::Free()
{
	auto* pEngine = VulkanEngine::GetInstance();
	vkFreeCommandBuffers(pEngine->GetVkDevice(), m_CommandPool, 1, &m_VkCommandBuffer);
	vkDestroyFence(pEngine->GetVkDevice(), m_VkFence, nullptr);
}
