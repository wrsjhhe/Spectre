#pragma once
#include "SpectreApi.h"
#include <unordered_map>
#include "VulkanBuffer.h"
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"

BEGIN_NAMESPACE_SPECTRE

class PipelineBufferManager:public SpectreApi
{
public:
	DefineClass(PipelineBufferManager);

private:
	VulkanVertexBufferPtr								m_VertexBuffer;
	VulkanIndexBufferPtr								m_IndexBuffer;
	std::vector<VulkanBufferPtr>                        m_vecUniformBuffer;
	std::vector<VulkanBufferPtr>						m_vecDymamicUniformBuffer;
};

END_NAMESPACE_SPECTRE