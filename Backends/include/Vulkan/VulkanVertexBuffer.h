#pragma once
#include <memory>

BEGIN_NAMESPACE_SPECTRE
class VulkanBuffer;
class VulkanVertexBuffer
{
public:
	static std::shared_ptr<VulkanVertexBuffer> Create(float* vertices, uint32_t count);

public:
	void Synchronize(const VkCommandBuffer& commandBuffer);

	void CmdBind(const VkCommandBuffer& commandBuffer,const VkDeviceSize* pOffset);
private:
	VulkanVertexBuffer(float* vertices, uint32_t count);

private:
	std::shared_ptr<VulkanBuffer>   m_HostBuffer;
	std::shared_ptr<VulkanBuffer>   m_DeviceBuffer;
};

END_NAMESPACE_SPECTRE