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

//void Renderer::BindCamera(PerspectiveCamera* pCamera)
//{
//	m_PerspectiveCameraPtr = pCamera;
//}

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

	m_Prepared = true;

}
#include <thread>
void Renderer::Render()
{
	ReadyScene();
	m_ScenePtr->UpdateCamera();

	//std::thread t([this]() {
	//	m_ScenePtr->UpdateUBO();
	//});
	//t.detach();
	m_ScenePtr->UpdateUBO();

	m_pRenderSystem->Draw();
}

void Renderer::ReadyScene()
{
	if (m_ScenePtr->NeedUpdateMeshBuffer())
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
				vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pCurPipeline->GetVkPipelineLayout(), 0, 1, 
					&m_ScenePtr->m_CameraData.DescriptorSet->GetVkSet(), 0, nullptr);
				//pCurPipeline->BindDescriptorSets(cmdBuffer);
				pCurPipeline->BindPipeline(cmdBuffer);
			}

			VkDeviceSize offsets[1] = { 0 };
			vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &pBatch->GPUBuffer.VertexBuffer->GetVkBuffer(), offsets);
			vkCmdBindIndexBuffer(cmdBuffer, pBatch->GPUBuffer.IndexBuffer->GetVkBuffer(), 0, VK_INDEX_TYPE_UINT32);

			for (uint32_t i = 0; i < pBatch->IndirectCommands.size(); ++i)
			{
				vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pCurPipeline->GetVkPipelineLayout(), 1, 1,
					&pBatch->Objects[i]->DescriptorSet->GetVkSet(), 0, nullptr);
				vkCmdDrawIndexedIndirect(cmdBuffer, pBatch->IndirectBuffer->GetVkBuffer(),
					i * sizeof(VkDrawIndexedIndirectCommand), 1, sizeof(VkDrawIndexedIndirectCommand));

			}
			/*if (VulkanEngine::GetInstance()->GetVkPhysicalDeviceFeatures().multiDrawIndirect)
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
			}*/
		}
	});
}
