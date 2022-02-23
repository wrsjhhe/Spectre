#pragma once
#include "SpectreDefs.h"
#include "SceneNode.h"
BEGIN_NAMESPACE_SPECTRE

class Scene
{
public:
	Scene();
	~Scene();

	void AddNode(SceneNode* pNode);

	SceneNode* GetRootNode();

private:
	SceneNode*				m_RootNode;

};

END_NAMESPACE_SPECTRE