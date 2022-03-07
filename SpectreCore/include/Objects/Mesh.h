#pragma once
#include "Object3D.h"
#include "Geometries/BufferGeometry.h"
#include "Materials/BufferMaterial.h"
#include "Math/MathDef.h"

BEGIN_NAMESPACE_SPECTRE

class Mesh : public Object3D
{
public:
	DefineClass(Mesh)
	static Mesh* Create(BufferGeometry* pGeometry, BufferMaterialPtr pMaterial);
public:

	virtual ~Mesh();

	BufferGeometry* GetBufferGeometry() const { return m_GeometryPtr; }
	BufferMaterialPtr GetMaterial() const { return m_MaterialPtr; }

	void Transform(const Matrix& mat);

	const Matrix& GetTransformMatrix()const { return m_Transform; }
private:
	Mesh(BufferGeometry* pGeometry, BufferMaterialPtr pMaterial);

private:
	BufferGeometry*					m_GeometryPtr;
	BufferMaterialPtr				m_MaterialPtr;
	Matrix                          m_Transform;
};

END_NAMESPACE_SPECTRE