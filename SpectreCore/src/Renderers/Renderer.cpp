#include "RenderSystemVK.h"
#include "VulkanEngine.h"
#include "Renderers/Renderer.h"

USING_NAMESPACE(Spectre)

Renderer::Renderer()
{


}

Renderer::~Renderer()
{
	delete m_pRenderSystem;
}

void Renderer::Attach(const NativeWindow& wnd)
{
	m_Window = wnd;
}


void Renderer::BindScene(Scene* pScene)
{
	m_ScenePtr = pScene;
}

void Renderer::BindCamera(PerspectiveCamera* pCamera)
{
	m_PerspectiveCameraPtr = pCamera;
}

void Renderer::Resize(uint32_t width, uint32_t height)
{
	m_Width = width;
	m_Height = height;
	if (m_Prepared)
	{
		SwapChainDesc swapChainDesc;
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;

		m_pRenderSystem->SetRect(width, height);
	}
}


void Renderer::Setup()
{
	RenderContextDesc renderDesc{};
	renderDesc.Window = m_Window;
	m_pRenderSystem = new RenderSystemVK();

	m_pRenderSystem->SetRect(m_Width, m_Height);

	m_pRenderSystem->CreateRenderContext(renderDesc);

	m_pRenderSystem->CreateSwapChain();

	CreateCameraUBO();
	m_Prepared = true;

}

void Renderer::Render()
{
	ReadyScene();
	
	//Test
	const std::vector<RenderBatch*>& batchs = m_ScenePtr->GetRenderBatchs();
	for (auto pBatch : batchs)
	{
		m_MVPData = m_PerspectiveCameraPtr->GetViewProjection();

		for (auto& obj : pBatch->Objects)
		{
			auto m = obj.MeshPtr->GetTransformMatrix();
			m_MVPData = m * m_MVPData;
		}
		auto& buffer = pBatch->Pipeline->GetUniformBuffer();
		if (buffer->MapPointerCache != nullptr)
		{
			std::memcpy(buffer->MapPointerCache, &m_MVPData, sizeof(Matrix)/*buffer->GetTotalSize()*/);
			buffer->Flush();
		}
		else
		{
			buffer->Map(&m_MVPData, sizeof(Matrix), 0, true);
		}
	}

	m_pRenderSystem->Draw();
}

void Renderer::ReadyScene()
{
	if (m_ScenePtr->NeedUpdate())
	{
		m_ScenePtr->PreparePipeline();
		m_ScenePtr->PrepareStageBuffer();
		m_ScenePtr->RefreshGPUBuffer();

		SetDrawCommandFunc();
		m_pRenderSystem->RecordDrawCommand();
	}
}

void Renderer::SetDrawCommandFunc()
{
	m_pRenderSystem->SetDrawCommand([this](VkCommandBuffer cmdBuffer)
	{
		VulkanPipelinePtr pCurPipeline = nullptr;
		const std::vector<RenderBatch*>& batchs = m_ScenePtr->GetRenderBatchs();
		for (auto pBatch : batchs)
		{
			if (pBatch->Pipeline->GetVkPipeline() == VK_NULL_HANDLE)
			{
				m_pRenderSystem->CreatePipeline(pBatch->Pipeline);
			}
			if (pCurPipeline != pBatch->Pipeline)
			{
				pCurPipeline = pBatch->Pipeline;
				pCurPipeline->BindDescriptorSets(cmdBuffer);
				pCurPipeline->BindPipeline(cmdBuffer);
			}

			VkDeviceSize offsets[1] = { 0 };
			vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &pBatch->GPUBuffer.VertexBuffer->GetVkBuffer(), offsets);
			vkCmdBindIndexBuffer(cmdBuffer, pBatch->GPUBuffer.IndexBuffer->GetVkBuffer(), 0, VK_INDEX_TYPE_UINT32);

			if (VulkanEngine::GetInstance()->GetVkPhysicalDeviceFeatures().multiDrawIndirect)
			{
				vkCmdDrawIndexedIndirect(cmdBuffer, pBatch->IndirectBuffer->GetVkBuffer(),
					0, pBatch->IndirectCommands.size(), sizeof(VkDrawIndexedIndirectCommand));
			}
			else
			{
				for (uint32_t i = 0; i < pBatch->IndirectCommands.size(); ++i)
				{
					vkCmdDrawIndexedIndirect(cmdBuffer, pBatch->IndirectBuffer->GetVkBuffer(),
						i * sizeof(VkDrawIndexedIndirectCommand), 1, sizeof(VkDrawIndexedIndirectCommand));

				}
			}
		}
	});
}


void Renderer::CreateCameraUBO()
{
	m_CameraData.Buffer = VulkanBuffer::Create(sizeof(CameraMatrix), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

	VkDescriptorBufferInfo cameraDescriptor;
	cameraDescriptor.buffer = m_CameraData.Buffer->GetVkBuffer();
	cameraDescriptor.offset = 0;
	cameraDescriptor.range = sizeof(CameraMatrix);

	m_CameraData.Descriptor = VulkanDescriptor::Create();
	m_CameraData.Descriptor->BindBuffer(1, &cameraDescriptor, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	m_CameraData.Descriptor->Build();
}
