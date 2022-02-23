#pragma once
#include <string>
#include <vector>
#include "SpectreDefs.h"
#include "Noncopyable.h"
#include "Geometry/Mesh.h"
BEGIN_NAMESPACE_SPECTRE

class Scene;
class SceneNode:public Noncopyable
{
	friend class Scene;
public:
	SceneNode* CreateChildNode();

	
	uint64_t GetNodeId();

	void SetNodeName(const std::string& strName);

	const std::string& GetNodeName();

	void AddMesh(Mesh* pMesh);

	std::vector<Mesh*> TraverseMeshes();

	~SceneNode();
private:
	SceneNode();

private:
	uint64_t                    m_Id = 0;
	std::string                 m_Name = "";
	SceneNode*					m_Parent = nullptr;
	std::vector<SceneNode*>     m_Children;
	std::vector<Mesh*>			m_Meshes;
	
};

END_NAMESPACE_SPECTRE