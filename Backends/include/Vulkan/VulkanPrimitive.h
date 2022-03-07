#pragma once
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"

BEGIN_NAMESPACE_SPECTRE

class VulkanPrimitive
{
public:
	std::shared_ptr<VulkanVertexBuffer>		VertexBufferPtr;

	std::shared_ptr <VulkanIndexBuffer>		IndicesBufferPtr;

	std::shared_ptr<VulkanPipeline>    PipelinePtr;

	uint32_t                                IndicesCount;

	std::shared_ptr<VulkanBuffer>           UniformBuffer;
public:

	void BindDrawCmd(const VkCommandBuffer& cmdBuffer);
};

END_NAMESPACE_SPECTRE