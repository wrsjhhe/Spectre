#include "Scenes/Scene.h"

USING_NAMESPACE(Spectre)

Scene::Scene()
{

}

Scene::~Scene()
{

}


void Scene::AddMesh(MeshPtr pMesh)
{
	RenderObject newObj;
	newObj.MeshPtr = pMesh;
	newObj.transformMatrix = pMesh->GetTransformMatrix();
	newObj.updateIndex = (uint32_t)-1;
	newObj.customSortKey = 0;
	m_PendingObjects.push_back(std::move(newObj));

	//m_Meshes.emplace_back(pMesh);
}

void Scene::PrepareStageBuffer()
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

			totalVertices += pGeomtry->VerticesCount();
			totalIndices += pGeomtry->IndicesCount();
		}

		m_MergedVertexBuffer.Alloc(totalVertices * sizeof(Vertex));
		m_MergedIndexBuffer.Alloc(totalIndices * sizeof(uint32_t));
	}
}



