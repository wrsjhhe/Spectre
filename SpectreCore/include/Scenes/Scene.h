#pragma once
#include <vector>
#include "Object3D.h"
#include "Objects/Mesh.h"
#include "VulkanPipeline.h"
#include "VulkanDescriptor.h"
#include "Cameras/PerspectiveCamera.h"

BEGIN_NAMESPACE_SPECTRE

struct RenderObject
{
	MeshPtr MeshPtr;

	uint32_t FirstIndex = 0;
	uint32_t FirstVertex = 0;

	VulkanPipelinePtr Pipeline;

	uint32_t UboOffset = 0;
};

struct MeshBuffer
{
	VulkanBufferPtr VertexBuffer;
	VulkanBufferPtr IndexBuffer;
};

struct RenderBatch
{
	~RenderBatch()
	{
		for (auto& o : Objects)
		{
			delete o;
		}
	}

	std::vector<RenderObject*> Objects;
	std::vector<VkDrawIndexedIndirectCommand> IndirectCommands;
	VulkanBufferPtr IndirectBuffer;
	VulkanBufferPtr UBOBuffer;

	std::vector<Vertex> Vertices;
	std::vector<uint32_t> Indices;

	bool NeedUpdate = false;

	VulkanPipelinePtr Pipeline;

	MeshBuffer GPUBuffer;

	VulkanDescriptorSetPtr DescriptorSet;

	uint32_t ObjectUboOffset = 0;
};

struct CameraMatrix
{
	Matrix View;
	Matrix Projection;
};

struct CameraData
{
	VulkanBufferPtr Buffer;
	VulkanDescriptorSetPtr DescriptorSet;
};

struct ModelData
{
	Matrix Transform;
};

class Scene : Object3D
{
public:
	DefineClass(Scene)
	Scene();
	virtual ~Scene();

	void AddMesh(MeshPtr pMesh);

	void AddCamera(PerspectiveCamera* pCamera);

	void RemoveMesh(SpectreId meshId);

	void PreparePipeline();

	void PrepareStageBuffer();

	void RefreshGPUBuffer();

	//const std::vector<RenderObject>& GetPassObjects() const { return m_PassObjects; }

	const std::vector<RenderBatch*>& GetRenderBatchs() const { return m_PassBatchs; }

	void UpdateCamera();

	bool NeedUpdateMeshBuffer() const { return m_MeshBufferChanged; }

	void UpdateUBO();

	CameraData									m_CameraData;

private:
	std::vector<RenderObject*> m_PendingObjects;
	std::vector<SpectreId>	   m_RemovingObjects;
	std::vector<RenderObject*> m_ModifyObjects;

	//std::vector<RenderObject> m_PassObjects;
	std::vector<RenderBatch*> m_PassBatchs;

	std::unordered_map<size_t, VulkanPipelinePtr> m_MatPipelineMap;

	bool m_MeshBufferChanged = false;

	std::unordered_map<SpectreId, RenderBatch*> m_MeshInBatchCache;

	VulkanDescriptorBuilder						m_CameraDescBuilder;
	VulkanDescriptorBuilder						m_ModelDescBuilder;

	PerspectiveCamera*							m_PerspectiveCameraPtr = nullptr;
};

END_NAMESPACE_SPECTRE