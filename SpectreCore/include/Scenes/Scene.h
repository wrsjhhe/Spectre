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

	uint32_t FirstIndex;
	uint32_t FirstVertex;

	VulkanPipelinePtr Pipeline;
	VkDescriptorSet DescriptorSet;
	VulkanBufferPtr ModelBuffer;
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

struct CameraMatrix
{
	Matrix View;
	Matrix Projection;
};

struct CameraData
{
	VulkanBufferPtr Buffer;
	VkDescriptorSet DescriptorSet;
};

class Scene : Object3D
{
public:
	DefineClass(Scene)
	Scene();
	virtual ~Scene();

	void AddMesh(MeshPtr pMesh);

	void AddCamera(PerspectiveCamera* pCamera);

	void RemoveMesh(MeshPtr pMesh);

	void PreparePipeline();

	void PrepareStageBuffer();

	void RefreshGPUBuffer();

	//const std::vector<RenderObject>& GetPassObjects() const { return m_PassObjects; }

	const std::vector<RenderBatch*>& GetRenderBatchs() const { return m_PassBatchs; }

	void UpdateCamera();

	bool NeedUpdate() const { return m_NeedUpdate; }
	CameraData									m_CameraData;
private:
	void CreateCameraDescriptor();

	void CreateModelDescriptor();

private:
	std::vector<RenderObject> m_PendingObjects;
	std::vector<RenderObject> m_RemovingObjects;
	std::vector<RenderObject> m_ModifyObjects;

	//std::vector<RenderObject> m_PassObjects;
	std::vector<RenderBatch*> m_PassBatchs;

	std::unordered_map<size_t, VulkanPipelinePtr> m_MatPipelineMap;

	bool m_NeedUpdate = false;

	std::unordered_map<SpectreId, RenderBatch*> m_MeshInBatchCache;

	
	VulkanDescriptorBuilder						m_CameraDescBuilder;
	VulkanDescriptorBuilder						m_ModelDescBuilder;

	PerspectiveCamera*							m_PerspectiveCameraPtr = nullptr;
};

END_NAMESPACE_SPECTRE