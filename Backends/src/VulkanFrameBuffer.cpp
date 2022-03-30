#include "VulkanRenderPass.h"
#include "VulkanFrameBuffer.h"

USING_NAMESPACE(Spectre)

std::shared_ptr<VulkanFrameBuffer> VulkanFrameBuffer::CreateFrameBuffer(const VulkanRenderPass& renderPass,
	const std::vector<VkImageView>& attachments,uint32_t width, uint32_t height)
{
	auto* pBuffer = new VulkanFrameBuffer(renderPass, attachments,width,height);
	return std::shared_ptr<VulkanFrameBuffer>(pBuffer);
}

VulkanFrameBuffer::~VulkanFrameBuffer()
{
	Destroy();
}

void VulkanFrameBuffer::Destroy()
{
	vkDestroyFramebuffer(VulkanEngine::GetInstance()->GetVkDevice(), m_FrameBuffers, nullptr);
}

VulkanFrameBuffer::VulkanFrameBuffer( const VulkanRenderPass& renderPass,
	const std::vector<VkImageView>& attachments, uint32_t width, uint32_t height)
{
	VkFramebufferCreateInfo frameBufferCreateInfo = {};
	frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCreateInfo.renderPass = renderPass.GetVkRenderPass();
	frameBufferCreateInfo.attachmentCount = attachments.size();
	frameBufferCreateInfo.pAttachments = attachments.data();
	frameBufferCreateInfo.width = width;
	frameBufferCreateInfo.height = height;
	frameBufferCreateInfo.layers = 1;

	vkCreateFramebuffer(VulkanEngine::GetInstance()->GetVkDevice(), &frameBufferCreateInfo, nullptr, &m_FrameBuffers);
}
