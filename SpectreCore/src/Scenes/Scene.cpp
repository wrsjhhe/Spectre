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
	RenderObject newObj;
	newObj.transformMatrix = pMesh->GetTransformMatrix();
	newObj.MaterialPtr = pMesh->GetMaterial();
	newObj.updateIndex = (uint32_t)-1;
	newObj.customSortKey = 0;
	m_PendingObjects.push_back(std::move(newObj));

	m_Meshes.emplace_back(pMesh);
}

void Scene::ReflashPass()
{
	if (!m_PendingObjects.empty())
	{
		uint32_t totalVertices = 0;
		uint32_t totalIndices = 0;
		for (auto& pendingObj : m_PendingObjects)
		{
			auto pGeomtry = pendingObj.MeshPtr->GetBufferGeometry();
			pendingObj.FirstVertex = totalVertices;
			pendingObj.FirstIndex = totalIndices;

			totalVertices += pendingObj.FirstVertex;
			totalIndices += pendingObj.FirstIndex;
		}

		m_MergedVertexBuffer.Alloc(totalVertices * sizeof(Vertex));
		m_MergedIndexBuffer.Alloc(totalIndices * sizeof(uint32_t));
	}
}



