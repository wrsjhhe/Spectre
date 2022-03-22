#include "Scenes/Scene.h"

USING_NAMESPACE(Spectre)

Scene::Scene()
{

}

Scene::~Scene()
{
	for (auto pBatch : m_PassBatchs)
	{
		delete pBatch;
		pBatch = nullptr;
	}
	m_PassBatchs.clear();
}


void Scene::AddMesh(MeshPtr pMesh)
{
	RenderObject newObj;
	newObj.MeshPtr = pMesh;
	m_PendingObjects.push_back(newObj);

	m_NeedUpdate = true;
}
struct UBOData
{
	Matrix MVPMatrix;
};
void Scene::PreparePipeline()
{
	if (m_PendingObjects.empty())
	{
		return;
	}

	for (auto& pendingObj : m_PendingObjects)
	{
		BufferMaterialPtr pMaterial = pendingObj.MeshPtr->GetMaterial();
		size_t matHash = pMaterial->GetHash();

		auto iter = m_MatPipelineMap.find(matHash);
		if (iter !=m_MatPipelineMap.end())
		{
			pendingObj.Pipeline = iter->second;
		}
		else
		{
			VulkanPipelinePtr newPipeline = VulkanPipeline::Create();
			newPipeline->CreateShaderModules({ pMaterial->GetVertexShader() }, { pMaterial->GetFragmentShader() });
			newPipeline->SetVertexDescription(pMaterial->GetAttributes());
			newPipeline->CreateUniformBuffer(sizeof(Matrix));

			pendingObj.Pipeline = newPipeline;
			m_MatPipelineMap[matHash] = newPipeline;
		}
	}

}

void Scene::PrepareStageBuffer()
{
	if (!m_PendingObjects.empty())
	{
		for (auto& pendingObj : m_PendingObjects)
		{
			RenderBatch* pPendingBatch = nullptr;
			for (auto pBatch : m_PassBatchs)
			{
				if (pBatch->Pipeline == pendingObj.Pipeline)
				{
					pPendingBatch = pBatch;
					break;;
				}
			}
			if (pPendingBatch == nullptr)
			{
				pPendingBatch = new RenderBatch;
				pPendingBatch->Pipeline = pendingObj.Pipeline;
				m_PassBatchs.push_back(pPendingBatch);
			}
		
			pPendingBatch->needupdate = true;
			pendingObj.FirstVertex = pPendingBatch->Vertices.size();
			pendingObj.FirstIndex = pPendingBatch->Indices.size();
			auto pGeomtry = pendingObj.MeshPtr->GetBufferGeometry();

			for (uint32_t i = 0;i < pGeomtry->VerticesCount();++i)
			{
				pPendingBatch->Vertices.push_back(pGeomtry->Vertices()[i]);
			}

			for (uint32_t i = 0; i < pGeomtry->IndicesCount(); ++i)
			{
				pPendingBatch->Indices.push_back(pGeomtry->Indices()[i]);
			}

			pPendingBatch->Objects.push_back(std::move(pendingObj));
		}
		m_PendingObjects.clear();

		for (auto pBatch : m_PassBatchs)
		{
			if (pBatch->needupdate == false)
			{
				continue;
			}
			uint32_t vertBufSize = pBatch->Vertices.size() * sizeof(Vertex);
			uint32_t indexBufSize = pBatch->Indices.size() * sizeof(uint32_t);
		}
	}
}

void Scene::RefreshGPUBuffer()
{
	for (auto pBatch : m_PassBatchs)
	{
		if (pBatch->needupdate == false)
		{
			continue;
		}		
		uint32_t vertBufSize = pBatch->Vertices.size() * sizeof(Vertex);
		uint32_t indexBufSize = pBatch->Indices.size() * sizeof(uint32_t);
		auto stageVertexBufferPtr = VulkanBuffer::Create(vertBufSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stageVertexBufferPtr->Map(pBatch->Vertices.data(), vertBufSize, 0);

		auto stageIndexBufferPtr = VulkanBuffer::Create(indexBufSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stageIndexBufferPtr->Map(pBatch->Indices.data(), indexBufSize, 0);

		pBatch->GPUBuffer.VertexBuffer = VulkanBuffer::Create(vertBufSize, VkBufferUsageFlagBits(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT),
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		pBatch->GPUBuffer.IndexBuffer = VulkanBuffer::Create(vertBufSize, VkBufferUsageFlagBits(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT),
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		stageVertexBufferPtr->CopyTo(pBatch->GPUBuffer.VertexBuffer);
		stageIndexBufferPtr->CopyTo(pBatch->GPUBuffer.IndexBuffer);

		pBatch->IndirectCommands.clear();
		for (auto& obj : pBatch->Objects)
		{
			VkDrawIndexedIndirectCommand command;
			command.firstInstance = 0;
			command.instanceCount = 1;
			command.firstIndex = obj.FirstIndex;
			command.vertexOffset = obj.FirstVertex;
			command.indexCount = obj.MeshPtr->GetBufferGeometry()->IndicesCount();
			pBatch->IndirectCommands.push_back(std::move(command));
		}

		VulkanBufferPtr indirectStagingBuffer = VulkanBuffer::Create(pBatch->IndirectCommands.size() * sizeof(VkDrawIndexedIndirectCommand),
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);

		indirectStagingBuffer->Map(pBatch->IndirectCommands.data(), pBatch->IndirectCommands.size() * sizeof(VkDrawIndexedIndirectCommand), 0);

		pBatch->IndirectBuffer = VulkanBuffer::Create(pBatch->IndirectCommands.size() * sizeof(VkDrawIndexedIndirectCommand),
			VkBufferUsageFlagBits(VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		indirectStagingBuffer->CopyTo(pBatch->IndirectBuffer);
	}
	m_NeedUpdate = false;
}

