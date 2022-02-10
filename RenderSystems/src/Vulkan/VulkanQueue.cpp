#include "VulkanCommon.h"
#include "VulkanQueue.h"

USING_NAMESPACE(Spectre)

VulkanQueue::VulkanQueue()
{

}


VulkanQueue::VulkanQueue(const VkDevice& device, uint32_t familyIndex):
	m_QueueFamilyIndex(familyIndex)
{
	vkGetDeviceQueue(device, m_QueueFamilyIndex, 0, &m_VkQueue);
}

VulkanQueue::VulkanQueue(const VulkanQueue& other)
{
	m_QueueFamilyIndex = other.m_QueueFamilyIndex;
	m_VkQueue = other.m_VkQueue;
}


