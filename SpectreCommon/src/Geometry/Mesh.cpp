#include "CommonDefinitions.h"
#include "Geometry/Mesh.h"
#include "Allocator.h"


USING_NAMESPACE(Spectre)

void Mesh::SetVertices(const Vertex* vertices, uint32_t count)
{
	uint32_t size = sizeof(Vertex) * count;
	m_Vertice = (Vertex*)HEAP_ALLOC(size);
	std::memcpy(m_Vertice, vertices, size);

	m_VertexCount = count;
}

void Mesh::SetFaceIndex(const uint32_t* faces, uint32_t count)
{
	uint32_t size = sizeof(uint32_t) * count;
	m_Faces = (uint32_t*)heap_alloc(sizeof(uint32_t) * count);
	std::memcpy(m_Faces, faces, size);

	m_FaceCount = count;
}

Mesh::~Mesh()
{
	HEAP_Free(m_Vertice);
	HEAP_Free(m_Faces);
}
