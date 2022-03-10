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
	Spectre::free(m_Vertices);
	Spectre::free(m_Indices);
}


void BufferGeometry::SetVertices(const Vertex* vertices, uint32_t count)
{
	uint32_t size = sizeof(Vertex) * count;
	m_Vertices = (Vertex*)heap_alloc(size);
	std::memcpy(m_Vertices, vertices, size);

	m_VerticesCount = count;
}

void BufferGeometry::SetFaceIndex(const uint32_t* indices, uint32_t count)
{
	uint32_t size = sizeof(uint32_t) * count;
	m_Indices = (uint32_t*)heap_alloc(sizeof(uint32_t) * count);
	std::memcpy(m_Indices, indices, size);

	m_IndicesCount = count;
}



