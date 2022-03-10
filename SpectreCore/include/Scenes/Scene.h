#pragma once
#include <vector>
#include "Object3D.h"
#include "Objects/Mesh.h"
BEGIN_NAMESPACE_SPECTRE

struct RenderObject {

	Mesh* MeshPtr;
	BufferMaterial* MaterialPtr;

	uint32_t updateIndex;
	uint32_t customSortKey{ 0 };

	Matrix transformMatrix;
};

class Scene : Object3D
{
public:
	DefineClass(Scene)
	Scene();
	virtual ~Scene();

	void Add(Object3D* pObj);

	const std::vector<Mesh*>& GetMeshes() const { return m_Meshes; }

private:
	
	void AddMesh(Mesh* pMesh);
private:
	std::vector<Mesh*> m_Meshes;
};

END_NAMESPACE_SPECTRE