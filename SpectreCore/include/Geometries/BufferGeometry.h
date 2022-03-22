#pragma once
#include <vector>
#include <memory>
#include <RenderTypes.h>
#include "Math/MathDef.h"
BEGIN_NAMESPACE_SPECTRE

class BufferGeometry;
typedef std::shared_ptr<BufferGeometry> BufferGeometryPtr;
class BufferGeometry
{
public:
	static BufferGeometry* Create(const std::vector<VertexAttribute>& vertexAttrs);
public:
	void SetVertexAttributes();
	//const std::vector<VertexAttribute>& VertexAttributes() const { return m_VertexAttrs; }

	void SetVertices(const Vertex* vertices, uint32_t count);
	void SetFaceIndex(const uint32_t* indices, uint32_t count);

	Vertex* Vertices() { return m_Vertices; }
	uint32_t* Indices() { return m_Indices; }

	uint32_t VerticesCount() { return m_VerticesCount; }
	uint32_t IndicesCount() { return m_IndicesCount; }


	~BufferGeometry();
private:
	BufferGeometry();

private:
	std::vector<VertexAttribute> m_VertexAttrs;

	uint32_t			m_VerticesCount = 0;
	Vertex*				m_Vertices = nullptr;

	uint32_t			m_IndicesCount = 0;
	uint32_t*			m_Indices = nullptr;

};

END_NAMESPACE_SPECTRE