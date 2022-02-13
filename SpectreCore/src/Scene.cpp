#include "Scene.h"

USING_NAMESPACE(Spectre)

Scene::Scene()
{
	m_RootNode = new SceneNode();
}

Scene::~Scene()
{
	delete m_RootNode;
}

void Scene::AddNode(SceneNode* pNode)
{
	pNode->m_Parent = this->m_RootNode;
}

SceneNode* Scene::GetRootNode()
{
	return m_RootNode;
}

