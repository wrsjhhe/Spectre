#include "Objects/Mesh.h"
USING_NAMESPACE(Spectre)

Mesh* Mesh::Create(BufferGeometry* pGeometry, Material* pMaterial)
{
	Mesh* pMesh = new Mesh(pGeometry, pMaterial);
	return pMesh;
}

Mesh::Mesh(BufferGeometry* pGeometry, Material* pMaterial):
	m_GeometryPtr(pGeometry),
	m_MaterialPtr(pMaterial)
{

}



Mesh::~Mesh()
{
	delete m_GeometryPtr;
	m_GeometryPtr = nullptr;
	delete m_MaterialPtr;
	m_MaterialPtr = nullptr;
}