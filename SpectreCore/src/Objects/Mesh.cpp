#include "Objects/Mesh.h"
USING_NAMESPACE(Spectre)

MeshPtr Mesh::Create(BufferGeometry* pGeometry, MeshBasicMaterialPtr pMaterial)
{
	Mesh* pMesh = new Mesh(pGeometry, pMaterial);
	return std::shared_ptr<Mesh>(pMesh);
}

Mesh::Mesh(BufferGeometry* pGeometry, MeshBasicMaterialPtr pMaterial):
	m_GeometryPtr(pGeometry),
	m_MaterialPtr(pMaterial)
{

}

Mesh::~Mesh()
{
}

void Mesh::Transform(const Matrix& mat)
{
	m_Transform *= mat;
	m_MatrixWorldNeedsUpdate = true;
}
