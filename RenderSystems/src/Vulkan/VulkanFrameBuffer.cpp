#include "VulkanCommon.h"
#include "VulkanDevice.h"
#include "VulkanRenderPass.h"
#include "VulkanFrameBuffer.h"

USING_NAMESPACE(Spectre)

std::shared_ptr<VulkanFrameBuffer> VulkanFrameBuffer::CreateFrameBuffer(const VulkanDevice& vulkanDevice, const VulkanRenderPass& renderPass,
	const std::vector<VkImageView>& attachments,uint32_t width, uint32_t height)
{
	auto* pBuffer = new VulkanFrameBuffer(vulkanDevice, renderPass, attachments,width,height);
	return std::shared_ptr<VulkanFrameBuffer>(pBuffer);
}

VulkanFrameBuffer::~VulkanFrameBuffer()
{
	Destroy();
}

void VulkanFrameBuffer::Destroy()
{
	vkDestroyFramebuffer(m_Device.GetVkDevice(), m_FrameBuffers, nullptr);
}

VulkanFrameBuffer::VulkanFrameBuffer(const VulkanDevice& vulkanDevice, const VulkanRenderPass& renderPass,
	const std::vector<VkImageView>& attachments, uint32_t width, uint32_t height):
	m_Device(vulkanDevice)
{
	VkFramebufferCreateInfo frameBufferCreateInfo = {};
	frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCreateInfo.renderPass = renderPass.GetVkRenderPass();
	frameBufferCreateInfo.attachmentCount = attachments.size();
	frameBufferCreateInfo.pAttachments = attachments.data();
	frameBufferCreateInfo.width = width;
	frameBufferCreateInfo.height = height;
	frameBufferCreateInfo.layers = 1;

	vkCreateFramebuffer(m_Device.GetVkDevice(), &frameBufferCreateInfo, nullptr, &m_FrameBuffers);
}
