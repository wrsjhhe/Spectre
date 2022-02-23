#include "SpectreDefs.h"
#include "Geometry/Mesh.h"
#include "Allocator.h"


USING_NAMESPACE(Spectre)

void Mesh::SetVertices(const float* vertices, uint32_t count)
{
	uint32_t size = sizeof(float) * count;
	m_Vertices = (float*)HEAP_ALLOC(size);
	std::memcpy(m_Vertices, vertices, size);

	m_VerticesCount = count;
}

void Mesh::SetFaceIndex(const uint32_t* faces, uint32_t count)
{
	uint32_t size = sizeof(uint32_t) * count;
	m_Indices = (uint32_t*)heap_alloc(sizeof(uint32_t) * count);
	std::memcpy(m_Indices, faces, size);

	m_IndicesCount = count;
}

Mesh::~Mesh()
{
	HEAP_Free(m_Vertices);
	HEAP_Free(m_Indices);
}
