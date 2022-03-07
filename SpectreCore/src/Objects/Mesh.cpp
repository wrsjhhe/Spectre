#include "Objects/Mesh.h"
USING_NAMESPACE(Spectre)

Mesh* Mesh::Create(BufferGeometry* pGeometry, BufferMaterialPtr pMaterial)
{
	Mesh* pMesh = new Mesh(pGeometry, pMaterial);
	return pMesh;
}

Mesh::Mesh(BufferGeometry* pGeometry, BufferMaterialPtr pMaterial):
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
