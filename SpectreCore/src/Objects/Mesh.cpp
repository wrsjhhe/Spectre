
#include "Objects/Mesh.h"
USING_NAMESPACE(Spectre)

Mesh* Mesh::Create(BufferGeometry* pGeometry, MeshBasicMaterialPtr pMaterial)
{
	Mesh* pMesh = new Mesh(pGeometry, pMaterial);
	return pMesh;
}

Mesh::Mesh(BufferGeometry* pGeometry, MeshBasicMaterialPtr pMaterial):
	m_GeometryPtr(pGeometry),
	m_MaterialPtr(pMaterial)
{

}

Mesh::~Mesh()
{
	delete m_GeometryPtr;
	m_GeometryPtr = nullptr;
}

void Mesh::Transform(const Matrix& mat)
{
	m_Transform *= mat;
}
