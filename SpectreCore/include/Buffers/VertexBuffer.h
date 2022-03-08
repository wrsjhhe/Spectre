#pragma once
#include "Buffer.h"
#include <vector>
#include <memory>
BEGIN_NAMESPACE_SPECTRE
class VertexBuffer;
class Mesh;
typedef std::shared_ptr<VertexBuffer> VertexBufferPtr;

class VertexBuffer :public Buffer<float>,private std::enable_shared_from_this<VertexBuffer>
{
public:
	DefineClass(VertexBuffer);

	VertexBuffer();
	virtual ~VertexBuffer();
	void PushMesh(Mesh* pMesh, float* pVertices, uint32_t count);

	void RemoveMesh(Mesh* pMesh, uint32_t offset, uint32_t count);

private:
	uint32_t									m_VertexCount;
	std::vector<Mesh*>							m_Meshes;
};

END_NAMESPACE_SPECTRE