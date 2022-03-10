#pragma once
#include <memory>

BEGIN_NAMESPACE_SPECTRE

class VulkanVertexBuffer;
typedef std::shared_ptr<VulkanVertexBuffer> VulkanVertexBufferPtr;

class VulkanVertexBuffer
{
public:
	static VulkanVertexBufferPtr Create(float* vertices, uint32_t count);

public:
	void Synchronize(const VkCommandBuffer& commandBuffer);

	void CmdBind(const VkCommandBuffer& commandBuffer,const VkDeviceSize* pOffset);
private:
	VulkanVertexBuffer(float* vertices, uint32_t count);

private:
	VulkanBufferPtr   m_HostBuffer;
	VulkanBufferPtr   m_DeviceBuffer;
};

END_NAMESPACE_SPECTRE