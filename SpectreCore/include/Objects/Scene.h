#pragma once
#include <vector>
#include "Objects/Object3D.h"
#include "Objects/Mesh.h"
BEGIN_NAMESPACE_SPECTRE

class Scene : Object3D
{
public:
	SpectreObject(Scene)
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