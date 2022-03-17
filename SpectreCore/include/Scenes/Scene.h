#pragma once
#include <vector>
#include "Object3D.h"
#include "Objects/Mesh.h"
#include "Buffers/BufferBlock.h"
#include "VulkanPipeline.h"
BEGIN_NAMESPACE_SPECTRE

struct RenderObject 
{
	MeshPtr MeshPtr;

	uint32_t FirstIndex;
	uint32_t FirstVertex;

	uint32_t updateIndex;
	uint32_t customSortKey{ 0 };

	BufferBlock* Buffer;
};

class Scene : Object3D
{
public:
	DefineClass(Scene)
	Scene();
	virtual ~Scene();

	void AddMesh(MeshPtr pMesh);

	const std::vector<Mesh*>& GetMeshes() const { return m_Meshes; }

	void CreatePipeline();

	void PrepareStageBuffer();

	void RefreshGPUBuffer();



	const std::vector<RenderObject>& GetPassObjects() const { return m_PassObjects; }

public:
	VulkanPipelinePtr TestPipeline;
	std::vector<VkDrawIndexedIndirectCommand> TestIndirectCommands;
	VulkanBufferPtr TestIndirectBuffer;
	BufferBlock  m_MergedVertexBuffer;
	BufferBlock  m_MergedIndexBuffer;
private:
	std::vector<Mesh*> m_Meshes;

	std::vector<RenderObject> m_PendingObjects;
	std::vector<RenderObject> m_DeletedObjects;
	std::vector<RenderObject> m_ModifyObjects;

	std::vector<RenderObject> m_PassObjects;


};

END_NAMESPACE_SPECTRE