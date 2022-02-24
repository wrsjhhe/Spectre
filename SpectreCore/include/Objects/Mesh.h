#pragma once
#include "Object3D.h"
#include "Geometries/BufferGeometry.h"
#include "Materials/Material.h"

BEGIN_NAMESPACE_SPECTRE

class Mesh : public Object3D
{
public:
	static Mesh* Create(BufferGeometry* pGeometry, Material* pMaterial);
public:

	~Mesh();

	BufferGeometry* GetBufferGeometry() const { return m_GeometryPtr; }
	Material* GetMaterial() const { return m_MaterialPtr; }
private:
	Mesh(BufferGeometry* pGeometry, Material* pMaterial);

private:
	BufferGeometry*					m_GeometryPtr;
	Material*						m_MaterialPtr;
};

END_NAMESPACE_SPECTRE