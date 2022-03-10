#pragma once
#include "VulkanBuffer.h"
#include "VulkanPipeline.h"
BEGIN_NAMESPACE_SPECTRE

class VulkanPrimitive:public SpectreApi
{
public:
	DefineClass(SpectreApi)
public:
	std::shared_ptr<VulkanBuffer>			VertexBufferPtr;

	std::shared_ptr <VulkanBuffer>			IndicesBufferPtr;

	std::shared_ptr<VulkanPipeline>			PipelinePtr;

	uint32_t                                IndicesCount;

	std::shared_ptr<VulkanBuffer>           UniformBuffer;
public:

	void BindDrawCmd(const VkCommandBuffer& cmdBuffer);
};

END_NAMESPACE_SPECTRE