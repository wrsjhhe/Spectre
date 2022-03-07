#include "Scenes/Scene.h"

USING_NAMESPACE(Spectre)

Scene::Scene()
{

}

Scene::~Scene()
{

}

void Scene::Add(Object3D* pObj)
{
	if (Mesh::Desc(pObj))
	{
		AddMesh(static_cast<Mesh*>(pObj));
	}
	
}

void Scene::AddMesh(Mesh* pMesh)
{
	m_Meshes.emplace_back(pMesh);
}



