#pragma once
#include <memory>
#include "Vertex.h"
#include "Face.h"
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
	void SetVertices(const Vertex* vertices, uint32_t count);
	void SetFaceIndex(const Face* faces, uint32_t count);

	~Mesh();

private:
	uint32_t			m_VertexCount;
	Vertex*				m_Vertice = nullptr;

	uint32_t			m_FaceCount;
	Face*				m_Faces = nullptr;

};
END_NAMESPACE_SPECTRE