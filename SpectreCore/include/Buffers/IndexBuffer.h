#pragma once
#include "Buffer.h"
#include <vector>
#include <memory>
BEGIN_NAMESPACE_SPECTRE
class IndexBuffer;
class Mesh;
typedef std::shared_ptr<IndexBuffer> IndexBufferPtr;
class IndexBuffer :public Buffer<float>, private std::enable_shared_from_this<VertexBuffer>
{
public:
	DefineClass(IndexBuffer);

	IndexBuffer();
	virtual ~IndexBuffer();
	void PushMesh(Mesh* pMesh, uint32_t* pIndices, uint32_t count);

	void RemoveMesh(Mesh* pMesh, uint32_t offset, uint32_t count);

private:
	uint32_t									m_IndexCount;
	std::vector<Mesh*>							m_Meshes;
};

END_NAMESPACE_SPECTRE