#pragma once

BEGIN_NAMESPACE_SPECTRE
#include <memory>
#include <vector>
class VulkanFrameBuffer
{
public:
	static std::shared_ptr<VulkanFrameBuffer> CreateFrameBuffer(const VulkanRenderPass& renderPass,
		const std::vector<VkImageView>& attachments,uint32_t width,uint32_t height);
public:
	~VulkanFrameBuffer();
	VkFramebuffer GetVkFrameBuffer() { return m_FrameBuffers; }
	void Destroy();

private:
	VulkanFrameBuffer(const VulkanRenderPass& renderPass,
		const std::vector<VkImageView>& attachments, uint32_t width, uint32_t height);

private:
	VkFramebuffer						m_FrameBuffers;
};

END_NAMESPACE_SPECTRE