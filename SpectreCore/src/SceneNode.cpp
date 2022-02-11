#include "SceneNode.h"

USING_NAMESPACE(Spectre)

SceneNode::SceneNode()
{

}



void SceneNode::AddMesh(Mesh* pMesh)
{
	m_Meshes.emplace_back(pMesh);
}

std::vector<Mesh*> SceneNode::TraverseMeshes()
{
	return m_Meshes;
}

uint64_t SceneNode::GetNodeId()
{
	return m_Id;
}

void SceneNode::SetNodeName(const std::string& strName)
{
	m_Name = strName;
}

const std::string& SceneNode::GetNodeName()
{
	return m_Name;
}

