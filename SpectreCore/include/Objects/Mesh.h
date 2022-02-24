#pragma once
#include "Object3D.h"
#include "Geometries/BufferGeometry.h"
#include "Materials/Material.h"

BEGIN_NAMESPACE_SPECTRE

class Mesh : public Object3D
{
private:
	BufferGeometry					m_Geometry;
	Material						m_Material;
};

END_NAMESPACE_SPECTRE