#include "PipelineBufferManager.h"

USING_NAMESPACE(Spectre)

void PipelineBufferManager::AddMesh(Mesh* pMesh)
{
	auto pGeometry = pMesh->GetBufferGeometry();

	m_VertexBufferPtr->PushMesh(pMesh, pGeometry->Vertices(), pGeometry->VerticesCount());

	m_IndexBufferPtr->PushMesh(pMesh, pGeometry->Indices(), pGeometry->IndicesCount());
}

PipelineBufferManager::PipelineBufferManager()
{
	m_VertexBufferPtr = std::make_shared<VertexBuffer>();
	m_IndexBufferPtr = std::make_shared<IndexBuffer>();
}

PipelineBufferManager::~PipelineBufferManager()
{

}

