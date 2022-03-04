#pragma once
#include <memory>
#include "RenderDef.h"

BEGIN_NAMESPACE_SPECTRE

class VulkanVertexBuffer
{
public:
	static std::shared_ptr<VulkanVertexBuffer> Create(const std::vector<float>& vertices);

public:
	void Synchronize(const VkCommandBuffer& commandBuffer);

	void CmdBind(const VkCommandBuffer& commandBuffer,const VkDeviceSize* pOffset);
private:
	VulkanVertexBuffer(const std::vector<float>& vertices);

private:
	std::shared_ptr<VulkanBuffer>   m_HostBuffer;
	std::shared_ptr<VulkanBuffer>   m_DeviceBuffer;
};

END_NAMESPACE_SPECTRE