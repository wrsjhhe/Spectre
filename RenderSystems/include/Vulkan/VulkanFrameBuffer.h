#pragma once

BEGIN_NAMESPACE_SPECTRE

class VulkanFrameBuffer
{
public:

private:
	VulkanFrameBuffer(const VulkanDevice& vulkanDevice);

private:
	const VulkanDevice&					m_Device;
	VkFramebuffer						m_FrameBuffers;
};

END_NAMESPACE_SPECTRE