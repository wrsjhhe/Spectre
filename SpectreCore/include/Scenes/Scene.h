#pragma once
#include <vector>
#include "Object3D.h"
#include "Objects/Mesh.h"
#include "Buffers/BufferBlock.h"
BEGIN_NAMESPACE_SPECTRE

struct RenderObject 
{
	MeshPtr MeshPtr;
	BufferMaterialPtr MaterialPtr;

	uint32_t FirstIndex;
	uint32_t FirstVertex;

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

	void ReflashPass();
private:
	std::vector<Mesh*> m_Meshes;

	std::vector<RenderObject> m_PendingObjects;
	std::vector<RenderObject> m_DeletedObjects;
	std::vector<RenderObject> m_ModifyObjects;

	BufferBlock  m_MergedVertexBuffer;
	BufferBlock  m_MergedIndexBuffer;
};

END_NAMESPACE_SPECTRE