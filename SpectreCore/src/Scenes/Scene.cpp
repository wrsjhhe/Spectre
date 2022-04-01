#include "Scenes/Scene.h"

USING_NAMESPACE(Spectre)

Scene::Scene()
{
	m_CameraDescBuilder.AddBind(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	m_ModelDescBuilder.AddBind(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	m_ModelDescBuilder.AddBind(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);

	m_CameraData.Buffer = VulkanBuffer::Create(sizeof(CameraMatrix), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

	VkDescriptorBufferInfo cameraDescriptor;
	cameraDescriptor.buffer = m_CameraData.Buffer->GetVkBuffer();
	cameraDescriptor.offset = 0;
	cameraDescriptor.range = sizeof(CameraMatrix);
	m_CameraData.DescriptorSet = m_CameraDescBuilder.Build({ &cameraDescriptor });
}

Scene::~Scene()
{
	for (auto pBatch : m_PassBatchs)
	{
		delete pBatch;
		pBatch = nullptr;
	}
	m_PassBatchs.clear();
	m_CameraData.Buffer = nullptr;
}


void Scene::AddMesh(MeshPtr pMesh)
{
	RenderObject* pNewObj = new RenderObject();
	pNewObj->MeshPtr = pMesh;
	m_PendingObjects.push_back(pNewObj);

	m_MeshBufferChanged = true;
}

void Scene::AddCamera(PerspectiveCamera* pCamera)
{
	m_PerspectiveCameraPtr = pCamera;
}

void Scene::RemoveMesh(SpectreId meshId)
{
	m_RemovingObjects.push_back(meshId);

	m_MeshBufferChanged = true;
}


void Scene::PreparePipeline()
{
	if (m_PendingObjects.empty())
	{
		return;
	}

	for (auto& pendingObj : m_PendingObjects)
	{
		BufferMaterialPtr pMaterial = pendingObj->MeshPtr->GetMaterial();
		size_t matHash = pMaterial->GetHash();

		auto iter = m_MatPipelineMap.find(matHash);
		if (iter !=m_MatPipelineMap.end())
		{
			pendingObj->Pipeline = iter->second;
		}
		else
		{
			VulkanPipelinePtr newPipeline = VulkanPipeline::Create();
			newPipeline->SetVertexShaders({ pMaterial->GetVertexShader() });
			newPipeline->SetFragmentShaders({ pMaterial->GetFragmentShader() });
			newPipeline->SetVertexDescription(pMaterial->GetAttributes());

			newPipeline->AddDescriptorSetLayout(m_CameraDescBuilder.GetOrCreateLayout());
			newPipeline->AddDescriptorSetLayout(m_ModelDescBuilder.GetOrCreateLayout());

			pendingObj->Pipeline = newPipeline;
			m_MatPipelineMap[matHash] = newPipeline;
		}
	}
}

void Scene::PrepareStageBuffer()
{
	if (!m_RemovingObjects.empty())
	{
		for (auto& removingObj : m_RemovingObjects)
		{
			auto iterBatch = m_MeshInBatchCache.find(removingObj);
			if (iterBatch != m_MeshInBatchCache.end())
			{
				RenderBatch* pBatch = iterBatch->second;
				for (uint32_t i = 0; i < pBatch->Objects.size(); ++i)
				{
					auto pObj = pBatch->Objects[i];
					if (pObj->MeshPtr->Id() == removingObj)
					{
						pBatch->Objects.erase(pBatch->Objects.begin() + i);
						pBatch->NeedUpdate = true;
						delete pObj;
						break;
					}
				}
				m_MeshInBatchCache.erase(iterBatch);
			}
		}
		m_RemovingObjects.clear();
	}

	if (!m_PendingObjects.empty())
	{
		for (auto& pendingObj : m_PendingObjects)
		{
			auto iter = m_MeshInBatchCache.find(pendingObj->MeshPtr->Id());
			if (iter != m_MeshInBatchCache.end())
			{
				continue;
			}
			RenderBatch* pPendingBatch = nullptr;
			for (auto pBatch : m_PassBatchs)
			{
				if (pBatch->Pipeline == pendingObj->Pipeline)
				{
					pPendingBatch = pBatch;
					break;;
				}
			}
			if (pPendingBatch == nullptr)
			{
				pPendingBatch = new RenderBatch;
				pPendingBatch->Pipeline = pendingObj->Pipeline;
				m_PassBatchs.push_back(pPendingBatch);

			
			}
			
			pPendingBatch->NeedUpdate = true;
			pPendingBatch->Objects.push_back(pendingObj);
			m_MeshInBatchCache[pendingObj->MeshPtr->Id()] = pPendingBatch;
		}
		m_PendingObjects.clear();
	}

	for (auto pBatch : m_PassBatchs)
	{
		if (pBatch->NeedUpdate)
		{
			pBatch->Vertices.clear();
			pBatch->Indices.clear();

			for (auto& obj : pBatch->Objects)
			{
				obj->FirstVertex = pBatch->Vertices.size();
				obj->FirstIndex = pBatch->Indices.size();
				auto pGeomtry = obj->MeshPtr->GetBufferGeometry();

				for (uint32_t i = 0; i < pGeomtry->VerticesCount(); ++i)
				{
					pBatch->Vertices.push_back(pGeomtry->Vertices()[i]);
				}

				for (uint32_t i = 0; i < pGeomtry->IndicesCount(); ++i)
				{
					pBatch->Indices.push_back(pGeomtry->Indices()[i]);
				}

				uint32_t materialBufferSize = obj->MeshPtr->GetMaterial()->GetBufferInfo().Size;
				//uint32_t materialBufferSize = 64;
				obj->ModelBuffer = VulkanBuffer::Create(sizeof(ModelData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
				obj->MaterialBuffer = VulkanBuffer::Create(materialBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
				VkDescriptorBufferInfo descriptor1;
				descriptor1.buffer = obj->ModelBuffer->GetVkBuffer();
				descriptor1.offset = 0;
				descriptor1.range = sizeof(ModelData);

				VkDescriptorBufferInfo descriptor2;
				descriptor2.buffer = obj->MaterialBuffer->GetVkBuffer();
				descriptor2.offset = 0;
				descriptor2.range = materialBufferSize;

				obj->DescriptorSet = m_ModelDescBuilder.Build({ &descriptor1,&descriptor2 });

				Matrix mat;
				obj->ModelBuffer->Map(&mat,sizeof(ModelData),0);
				obj->MaterialBuffer->Map(obj->MeshPtr->GetMaterial()->GetBufferInfo().Buffer, materialBufferSize, 0);
			}
		}
	}
}

void Scene::RefreshGPUBuffer()
{
	for (auto pBatch : m_PassBatchs)
	{
		if (pBatch->NeedUpdate == false)
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
			command.firstIndex = obj->FirstIndex;
			command.vertexOffset = obj->FirstVertex;
			command.indexCount = obj->MeshPtr->GetBufferGeometry()->IndicesCount();
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

		pBatch->NeedUpdate = false;
	}
	m_MeshBufferChanged = false;
}

void Scene::UpdateCamera()
{
	CameraMatrix matrix
	{
		m_PerspectiveCameraPtr->GetView(),
		m_PerspectiveCameraPtr->GetProjection()
	};

	m_CameraData.Buffer->Map(&matrix, sizeof(CameraMatrix), 0);

}

void Scene::UpdateUBO()
{
	for (auto iter = m_MeshInBatchCache.begin();iter!=m_MeshInBatchCache.end();++iter)
	{
		auto& batch = iter->second;
		for (auto o : batch->Objects)
		{
			auto pMesh = o->MeshPtr;
			if (pMesh->MatrixWorldNeedsUpdate())
			{
				Matrix mat = pMesh->GetTransformMatrix();
				o->ModelBuffer->Map(&mat);
				pMesh->m_MatrixWorldNeedsUpdate = false;
			}
		}
	}

}
