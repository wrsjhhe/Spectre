#pragma once

BEGIN_NAMESPACE_SPECTRE
#include <memory>
#include <vector>
class VulkanFrameBuffer : public Noncopyable
{
public:
	static std::shared_ptr<VulkanFrameBuffer> CreateFrameBuffer(const VulkanDevice& vulkanDevice,const VulkanRenderPass& renderPass,
		const std::vector<VkImageView>& attachments,uint32_t width,uint32_t height);
public:
	~VulkanFrameBuffer();
	VkFramebuffer GetVkFrameBuffer() { return m_FrameBuffers; }
	void Destroy();

private:
	VulkanFrameBuffer(const VulkanDevice& vulkanDevice, const VulkanRenderPass& renderPass,
		const std::vector<VkImageView>& attachments, uint32_t width, uint32_t height);

private:
	const VulkanDevice&					m_Device;   
	VkFramebuffer						m_FrameBuffers;
};

END_NAMESPACE_SPECTRE