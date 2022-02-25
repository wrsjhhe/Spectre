#include "SpectreDef.h"
#include "Geometries/BufferGeometry.h"
#include "Allocator.h"
#include <memory>
USING_NAMESPACE(Spectre)


BufferGeometry* BufferGeometry::Create(const std::vector<VertexAttribute>& vertexAttrs)
{
	BufferGeometry* pGeometry = new BufferGeometry();
	pGeometry->m_VertexAttrs = vertexAttrs;
	return pGeometry;
}

BufferGeometry::BufferGeometry()
{

}

BufferGeometry::~BufferGeometry()
{
	HEAP_Free(m_Vertices);
	HEAP_Free(m_Indices);
}


void BufferGeometry::SetVertices(const float* vertices, uint32_t count)
{
	uint32_t size = sizeof(float) * count;
	m_Vertices = (float*)HEAP_ALLOC(size);
	std::memcpy(m_Vertices, vertices, size);

	m_VerticesCount = count;
}

void BufferGeometry::SetFaceIndex(const uint32_t* faces, uint32_t count)
{
	uint32_t size = sizeof(uint32_t) * count;
	m_Indices = (uint32_t*)heap_alloc(sizeof(uint32_t) * count);
	std::memcpy(m_Indices, faces, size);

	m_IndicesCount = count;
}



