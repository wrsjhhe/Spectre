#pragma once
#include "Object3D.h"
#include "Geometries/BufferGeometry.h"
#include "Materials/Materials.h"
#include "Math/MathDef.h"

BEGIN_NAMESPACE_SPECTRE

class Mesh : public Object3D
{
public:
	SpectreObject(Mesh)
	static Mesh* Create(BufferGeometry* pGeometry, MeshBasicMaterialPtr pMaterial);
public:

	virtual ~Mesh();

	BufferGeometry* GetBufferGeometry() const { return m_GeometryPtr; }
	MeshBasicMaterialPtr GetMaterial() const { return m_MaterialPtr; }

	void Transform(const Matrix& mat);

	const Matrix& GetTransformMatrix()const { return m_Transform; }
private:
	Mesh(BufferGeometry* pGeometry, MeshBasicMaterialPtr pMaterial);

private:
	BufferGeometry*					m_GeometryPtr;
	MeshBasicMaterialPtr			m_MaterialPtr;
	Matrix                          m_Transform;
};

END_NAMESPACE_SPECTRE