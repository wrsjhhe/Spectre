#include "Scenes/Scene.h"

USING_NAMESPACE(Spectre)

Scene::Scene()
{

}

Scene::~Scene()
{

}


void Scene::AddMesh(MeshPtr pMesh)
{
	RenderObject newObj;
	newObj.MeshPtr = pMesh;
	newObj.updateIndex = (uint32_t)-1;
	newObj.customSortKey = 0;
	m_PendingObjects.push_back(newObj);

	//m_Meshes.emplace_back(pMesh);
}
struct UBOData
{
	Matrix MVPMatrix;
};
void Scene::CreatePipeline()
{
	BufferMaterialPtr pMaterial = m_PendingObjects[0].MeshPtr->GetMaterial();
	PipelineDesc pipelineDesc;
	pipelineDesc.VertexAttributes = pMaterial->GetAttributes();
	pipelineDesc.VertexShaders = { pMaterial->GetVertexShader() };
	pipelineDesc.FragmentShaders = { pMaterial->GetFragmentShader() };
	pipelineDesc.UniformBufferSizes = sizeof(UBOData);

	TestPipeline = VulkanPipeline::Create();
	TestPipeline->CreateShaderModules(pipelineDesc.VertexShaders, pipelineDesc.FragmentShaders);
	TestPipeline->SetVertexDescription(pipelineDesc.VertexAttributes);
	TestPipeline->CreateUniformBuffer(pipelineDesc.UniformBufferSizes);
}

void Scene::PrepareStageBuffer()
{
	if (!m_PendingObjects.empty())
	{
		std::vector<Vertex> totalVertices;
		std::vector<uint32_t> totalIndices;
		for (auto& pendingObj : m_PendingObjects)
		{
			pendingObj.FirstVertex = totalVertices.size();
			pendingObj.FirstIndex = totalIndices.size();
			auto pGeomtry = pendingObj.MeshPtr->GetBufferGeometry();

			for (uint32_t i = 0;i < pGeomtry->VerticesCount();++i)
			{
				totalVertices.push_back(pGeomtry->Vertices()[i]);
			}

			for (uint32_t i = 0; i < pGeomtry->IndicesCount(); ++i)
			{
				totalIndices.push_back(pGeomtry->Indices()[i]);
			}

			m_PassObjects.push_back(std::move(pendingObj));	
		}

		uint32_t vertBufSize = totalVertices.size() * sizeof(Vertex);
		uint32_t indexBufSize = totalIndices.size() * sizeof(uint32_t);

		m_MergedVertexBuffer.Alloc(vertBufSize);
		m_MergedIndexBuffer.Alloc(indexBufSize);

		m_MergedVertexBuffer.PushBuffer(totalVertices.data(), vertBufSize);
		m_MergedIndexBuffer.PushBuffer(totalIndices.data(), indexBufSize);
	}
}

void Scene::RefreshGPUBuffer()
{
	m_MergedVertexBuffer.Upload(VkBufferUsageFlagBits(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT));
	m_MergedIndexBuffer.Upload(VkBufferUsageFlagBits(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT));

	for (auto& obj : m_PassObjects)
	{
		VkDrawIndexedIndirectCommand command;
		command.firstInstance = 0;
		command.instanceCount = 0;
		command.firstIndex = obj.FirstIndex;
		command.vertexOffset = obj.FirstVertex;
		command.indexCount = obj.MeshPtr->GetBufferGeometry()->IndicesCount();
		TestIndirectCommands.push_back(std::move(command));
	}

	VulkanBufferPtr indirectStagingBuffer = VulkanBuffer::Create(TestIndirectCommands.size() * sizeof(VkDrawIndexedIndirectCommand),
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	indirectStagingBuffer->Map(TestIndirectCommands.data(), TestIndirectCommands.size() * sizeof(VkDrawIndexedIndirectCommand),0);

	TestIndirectBuffer = VulkanBuffer::Create(TestIndirectCommands.size() * sizeof(VkDrawIndexedIndirectCommand),
		VkBufferUsageFlagBits(VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);

	indirectStagingBuffer->CopyTo(TestIndirectBuffer);
}


