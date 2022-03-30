#include "RenderSystemVK.h"
#include "VulkanEngine.h"
#include "Renderers/Renderer.h"

USING_NAMESPACE(Spectre)

Renderer::Renderer()
{
	CreateRenderPass();
	CreateSwapchain();


}

Renderer::~Renderer()
{
	delete m_pRenderSystem;
}

void Renderer::Attach(const NativeWindow& wnd)
{
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hinstance = GetModuleHandle(NULL);
	surfaceCreateInfo.hwnd = (HWND)wnd.hWnd;
	VK_CHECK(vkCreateWin32SurfaceKHR(VulkanEngine::GetInstance()->GetVkInstance(), &surfaceCreateInfo, nullptr, &m_Surface), "Failed create vkSurface!");

	uint32_t surafceSupport = 0;
	vkGetPhysicalDeviceSurfaceSupportKHR(VulkanEngine::GetInstance()->GetVkPhysicalDevice(), 
		VulkanEngine::GetInstance()->GetGraphicQueue().QueueFamilyIndex, m_Surface, &surafceSupport);
	EXP_CHECK(surafceSupport, "Surface not support");
}


void Renderer::BindScene(Scene* pScene)
{
	m_ScenePtr = pScene;
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
	//RenderContextDesc renderDesc{};
	//renderDesc.Window = m_Window;
	m_pRenderSystem = new RenderSystemVK();

	m_pRenderSystem->SetRect(m_Width, m_Height);

	//m_pRenderSystem->CreateRenderContext(renderDesc);

	//m_pRenderSystem->CreateSwapChain();

	m_Prepared = true;

}

void Renderer::Render()
{
	ReadyScene();
	m_ScenePtr->UpdateCamera();

	m_ScenePtr->UpdateUBO();

	m_pRenderSystem->Draw();
}

void Renderer::CreateRenderPass()
{
	m_RenderPass = VulkanRenderPass::CreateCommonRenderPass(g_SurfaceFormat.format);
}

void Renderer::CreateSwapchain()
{
	if (m_Surface == nullptr)
	{
		return;
	}
	SwapChainDesc desc{ m_Width ,m_Height };
	m_SwapChain = std::make_shared<VulkanSwapChain>(m_Surface, desc);
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

void Renderer::SetDrawCommand()
{
	//VkClearValue clearValues[2];
	//clearValues[0].color = {
	//	{0.350f, 0.350f, 0.350f, 1.0f}
	//};
	//clearValues[1].depthStencil = { 1.0f, 0 };

	//VkRenderPassBeginInfo renderPassBeginInfo{};
	//renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	//renderPassBeginInfo.renderPass = m_RenderPass->GetVkRenderPass();
	//renderPassBeginInfo.clearValueCount = 2;
	//renderPassBeginInfo.pClearValues = clearValues;
	//renderPassBeginInfo.renderArea.offset.x = 0;
	//renderPassBeginInfo.renderArea.offset.y = 0;
	//renderPassBeginInfo.renderArea.extent.width = m_Width;
	//renderPassBeginInfo.renderArea.extent.height = m_Height;

	//auto cmds = VulkanEngine::GetInstance()->GetRenderCmd();
	//for (uint32_t i = 0; i < cmds.size(); ++i)
	//{
	//	renderPassBeginInfo.framebuffer = m_FrameBuffers[i]->GetVkFrameBuffer();

	//	VkViewport viewport = {};
	//	viewport.x = 0;
	//	viewport.y = (float)m_SwapChain->GetHeight();
	//	viewport.width = (float)m_SwapChain->GetWidth();
	//	viewport.height = -(float)m_SwapChain->GetHeight();    // flip y axis
	//	viewport.minDepth = 0.0f;
	//	viewport.maxDepth = 1.0f;

	//	VkRect2D scissor = {};
	//	scissor.extent.width = m_SwapChain->GetWidth();
	//	scissor.extent.height = m_SwapChain->GetHeight();
	//	scissor.offset.x = 0;
	//	scissor.offset.y = 0;

	//	if (!cmds[i]->IsOnRecording())
	//	{
	//		cmds[i]->Reset();
	//	}

	//	cmds[i]->RecordCommond([&](VkCommandBuffer cmdBuffer) {
	//		vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	//		vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
	//		vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
	//		m_DrawCommand(cmdBuffer);

	//		vkCmdEndRenderPass(cmdBuffer);
	//	});

	//}
	//vkDeviceWaitIdle(m_VulkanEnginePtr->GetVkDevice());
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
