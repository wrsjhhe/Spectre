#include "CommonDefinitions.h"
#include "Geometry/Mesh.h"
#include "Allocator.h"


USING_NAMESPACE(Spectre)

void Mesh::SetVertices(const Vertex* vertices, uint32_t count)
{
	uint32_t size = sizeof(Vertex) * count;
	m_Vertice = (Vertex*)heap_alloc(sizeof(Vertex) * count);
	std::memcpy(m_Vertice, vertices, size);
}

void Mesh::SetFaceIndex(const Face* faces, uint32_t count)
{
	uint32_t size = sizeof(Face) * count;
	m_Faces = (Face*)heap_alloc(sizeof(Face) * count);
	std::memcpy(m_Faces, faces, size);
}

Mesh::~Mesh()
{
	::free(m_Vertice);
	::free(m_Faces);
}
