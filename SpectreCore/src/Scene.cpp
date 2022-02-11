#include "Scene.h"

USING_NAMESPACE(Spectre)

Scene::Scene()
{
	m_RootNode = new SceneNode();
}

void Scene::AddNode(SceneNode* pNode)
{
	pNode->m_Parent = this->m_RootNode;
}

SceneNode* Scene::GetRootNode()
{
	return m_RootNode;
}

