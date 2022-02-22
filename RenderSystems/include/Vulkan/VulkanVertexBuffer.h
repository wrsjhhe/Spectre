#pragma once
#include <memory>
#include "RSDefs.h"

BEGIN_NAMESPACE_SPECTRE

class VulkanVertexBuffer
{
public:
	static std::shared_ptr<VulkanVertexBuffer> Create(const VulkanDevice& device,const std::vector<float>& vertices,
		const std::vector<VertexAttribute>& attributes);

public:
	void Synchronize(const VkCommandBuffer& commandBuffer);

	void CmdBind(const VkCommandBuffer& commandBuffer,const VkDeviceSize* pOffset);

private:
	VulkanVertexBuffer(const VulkanDevice& vulkanDevice, const std::vector<float>& vertices,
		const std::vector<VertexAttribute>& attributes);

private:
	std::shared_ptr<VulkanBuffer>   m_HostBuffer;
	std::shared_ptr<VulkanBuffer>   m_DeviceBuffer;
};

END_NAMESPACE_SPECTRE