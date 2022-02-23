#pragma once
#include <memory>
BEGIN_NAMESPACE_SPECTRE

//template <typename... _Types>
//class Mesh;
//
//template<typename _Vertex, typename _Face, typename... _Rest>
//class Mesh<_Vertex, _Face, _Rest...>
//{
//public:
//	uint32_t			m_VertexCount;
//	_Vertex*			m_Vertice;
//
//	uint32_t			m_FaceCount;
//	_Face*				m_Faces;
//};
//
//template<typename _Vertex, typename _Face, typename _Color, typename... _Rest>
//class Mesh<_Vertex, _Face, _Color, _Rest...> :public Mesh<_Vertex, _Face, _Rest...>
//{
//public:
//	_Color				m_VertexColor;
//	uint32_t			m_VertexColorCount;
//};

class Mesh
{
public:
	void SetVertices(const float* vertices, uint32_t count);
	void SetFaceIndex(const uint32_t* faces, uint32_t count);

	float* Vertices() { return m_Vertices; }
	uint32_t* Faces() { return m_Indices; }

	uint32_t VerticesCount() { return m_VerticesCount; }
	uint32_t FacesCount() { return m_IndicesCount; }


	~Mesh();

private:
	uint32_t			m_VerticesCount;
	float*				m_Vertices = nullptr;

	uint32_t			m_IndicesCount;
	uint32_t*			m_Indices = nullptr;

};
END_NAMESPACE_SPECTRE