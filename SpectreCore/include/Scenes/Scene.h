#pragma once
#include <vector>
#include "Object3D.h"
#include "Objects/Mesh.h"
#include "VulkanPipeline.h"
BEGIN_NAMESPACE_SPECTRE

struct RenderObject 
{
	MeshPtr MeshPtr;

	uint32_t FirstIndex;
	uint32_t FirstVertex;

	VulkanPipelinePtr Pipeline;
};

struct MeshBuffer
{
	VulkanBufferPtr VertexBuffer;
	VulkanBufferPtr IndexBuffer;
};

struct RenderBatch
{
	std::vector<RenderObject> Objects;
	std::vector<VkDrawIndexedIndirectCommand> IndirectCommands;
	VulkanBufferPtr IndirectBuffer;

	std::vector<Vertex> Vertices;
	std::vector<uint32_t> Indices;

	bool NeedUpdate = false;

	VulkanPipelinePtr Pipeline;

	MeshBuffer GPUBuffer;
};


class Scene : Object3D
{
public:
	DefineClass(Scene)
	Scene();
	virtual ~Scene();

	void AddMesh(MeshPtr pMesh);

	void RemoveMesh(MeshPtr pMesh);

	void PreparePipeline();

	void PrepareStageBuffer();

	void RefreshGPUBuffer();

	//const std::vector<RenderObject>& GetPassObjects() const { return m_PassObjects; }

	const std::vector<RenderBatch*>& GetRenderBatchs() const { return m_PassBatchs; }

	bool NeedUpdate() const { return m_NeedUpdate; }

private:
	std::vector<RenderObject> m_PendingObjects;
	std::vector<RenderObject> m_RemovingObjects;
	std::vector<RenderObject> m_ModifyObjects;

	//std::vector<RenderObject> m_PassObjects;
	std::vector<RenderBatch*> m_PassBatchs;

	std::unordered_map<size_t, VulkanPipelinePtr> m_MatPipelineMap;

	bool m_NeedUpdate = false;

	std::unordered_map<SpectreId, RenderBatch*> m_MeshInBatchCache;
};

END_NAMESPACE_SPECTRE