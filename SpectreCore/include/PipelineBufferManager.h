#pragma once
#include "SpectreApi.h"
#include <memory>
#include "Buffers/VertexBuffer.h"
#include "Buffers/IndexBuffer.h"
#include "Objects/Mesh.h"
BEGIN_NAMESPACE_SPECTRE

class PipelineBufferManager;
typedef std::shared_ptr<PipelineBufferManager> PipelineBufferManagerPtr;

class PipelineBufferManager:public SpectreApi
{
public:
	DefineClass(PipelineBufferManager);

	void AddMesh(Mesh* pMesh);

	PipelineBufferManager();
	~PipelineBufferManager();
private:
	VertexBufferPtr								m_VertexBufferPtr;
	IndexBufferPtr								m_IndexBufferPtr;
	//std::vector<VulkanBufferPtr>                        m_vecUniformBuffer;
	//std::vector<VulkanBufferPtr>						m_vecDymamicUniformBuffer;
};

END_NAMESPACE_SPECTRE