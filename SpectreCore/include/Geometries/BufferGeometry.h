#pragma once

BEGIN_NAMESPACE_SPECTRE

class BufferGeometry
{
public:
	void SetVertices(const float* vertices, uint32_t count);
	void SetFaceIndex(const uint32_t* faces, uint32_t count);

	float* Vertices() { return m_Vertices; }
	uint32_t* Indices() { return m_Indices; }

	uint32_t VerticesCount() { return m_VerticesCount; }
	uint32_t IndicesCount() { return m_IndicesCount; }


	~BufferGeometry();

private:
	uint32_t			m_VerticesCount;
	float* m_Vertices = nullptr;

	uint32_t			m_IndicesCount;
	uint32_t* m_Indices = nullptr;

};

END_NAMESPACE_SPECTRE