#include "VulkanEngine.h"
#include "Renderers/Renderer.h"

USING_NAMESPACE(Spectre)

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
	m_SwapChain = nullptr;
	if (m_Surface != nullptr)
	{
		vkDestroySurfaceKHR(VulkanEngine::GetInstance()->GetVkInstance(), m_Surface, nullptr);
	}
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
}


void Renderer::Setup()
{
	CreateRenderPass();
	CreateSwapchain();
	CreateFrameBuffer();

	m_RenderComplete = VulkanSemaphore::CreateSemaphore();

	m_Prepared = true;

}

void Renderer::Render()
{
	ReadyScene();
	m_ScenePtr->UpdateCamera();

	m_ScenePtr->UpdateUBO();

	VulkanSemaphorePtr presentComplete;
	VkDevice device = VulkanEngine::GetInstance()->GetVkDevice();
	VulkanQueue queue = VulkanEngine::GetInstance()->GetGraphicQueue();
	int32_t backBufferIndex = m_SwapChain->AcquireImageIndex(presentComplete);
	if (backBufferIndex < 0)
	{
		SwapChainDesc _desc{m_Width,m_Height};
		vkDeviceWaitIdle(device);

		m_FrameBuffers.clear();

		m_SwapChain = nullptr;

		vkDeviceWaitIdle(device);
		
		m_Width = _desc.Width;
		m_Height = _desc.Height;

		m_SwapChain = std::make_shared<VulkanSwapChain>(m_Surface, _desc);
		CreateFrameBuffer();

		SetDrawCommand();
		return;
	}

	// Ìá½»»æÖÆÃüÁî
	VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	auto cmd = VulkanEngine::GetInstance()->GetRenderCmd();
	cmd[backBufferIndex]->SignalSemaphore = { m_RenderComplete->GetVkSemaphore() };
	cmd[backBufferIndex]->WaitSemaphore = { presentComplete->GetVkSemaphore() };
	cmd[backBufferIndex]->WaitStageMask = { waitStageMask };

	cmd[backBufferIndex]->Submit(queue, false);

	// present
	m_SwapChain->Present(
		queue.VkQueue,
		&m_RenderComplete->GetVkSemaphore()
	);
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

void Renderer::CreateFrameBuffer()
{
	uint32_t fwidth = m_Width;
	uint32_t fheight = m_Height;
	VkDevice device = VulkanEngine::GetInstance()->GetVkDevice();

	m_DepthStencilImage = VulkanImage::Create(m_Width, m_Height, VK_FORMAT_D24_UNORM_S8_UINT, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
	m_DepthStencilImageView = VulkanImageView::Create(m_DepthStencilImage->GetVkImage(), VK_FORMAT_D24_UNORM_S8_UINT,
		VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);

	std::vector<VkImageView> attachments;
	attachments.resize(2);
	attachments[1] = m_DepthStencilImageView->GetVkImageView();

	const std::vector<VulkanImageViewPtr>& backbufferViews = m_SwapChain->GetImageView();

	m_FrameBuffers.resize(backbufferViews.size());
	for (uint32_t i = 0; i < m_FrameBuffers.size(); ++i)
	{
		attachments[0] = backbufferViews[i]->GetVkImageView();
		m_FrameBuffers[i] = VulkanFrameBuffer::CreateFrameBuffer(*m_RenderPass, attachments, m_Width, m_Height);
	}
}

void Renderer::ReadyScene()
{
	if (m_ScenePtr->NeedUpdateMeshBuffer())
	{
		m_ScenePtr->PreparePipeline();
		m_ScenePtr->PrepareStageBuffer();
		m_ScenePtr->RefreshGPUBuffer();

		SetDrawCommand();
	}
}

void Renderer::SetDrawCommand()
{
	VkClearValue clearValues[2];
	clearValues[0].color = {
		{0.350f, 0.350f, 0.350f, 1.0f}
	};
	clearValues[1].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo renderPassBeginInfo{};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = m_RenderPass->GetVkRenderPass();
	renderPassBeginInfo.clearValueCount = 2;
	renderPassBeginInfo.pClearValues = clearValues;
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent.width = m_Width;
	renderPassBeginInfo.renderArea.extent.height = m_Height;

	auto cmds = VulkanEngine::GetInstance()->GetRenderCmd();
	for (uint32_t i = 0; i < cmds.size(); ++i)
	{
		renderPassBeginInfo.framebuffer = m_FrameBuffers[i]->GetVkFrameBuffer();

		VkViewport viewport = {};
		viewport.x = 0;
		viewport.y = (float)m_SwapChain->GetHeight();
		viewport.width = (float)m_SwapChain->GetWidth();
		viewport.height = -(float)m_SwapChain->GetHeight();    // flip y axis
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.extent.width = m_SwapChain->GetWidth();
		scissor.extent.height = m_SwapChain->GetHeight();
		scissor.offset.x = 0;
		scissor.offset.y = 0;

		if (!cmds[i]->IsOnRecording())
		{
			cmds[i]->Reset();
		}

		cmds[i]->RecordCommond([&](VkCommandBuffer cmdBuffer) {

			vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
			vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
			VulkanPipelinePtr pCurPipeline = nullptr;
			const std::vector<RenderBatch*>& batchs = m_ScenePtr->GetRenderBatchs();
			for (auto pBatch : batchs)
			{
				if (pBatch->Pipeline->GetVkPipeline() == VK_NULL_HANDLE)
				{
					pBatch->Pipeline->CreatePipelineInstance(*m_RenderPass);
				}
				if (pCurPipeline != pBatch->Pipeline)
				{
					pCurPipeline = pBatch->Pipeline;
					vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pCurPipeline->GetVkPipelineLayout(), 0, 1,
						&m_ScenePtr->m_CameraData.DescriptorSet->GetVkSet(), 0, nullptr);
					pCurPipeline->BindPipeline(cmdBuffer);
				}

				VkDeviceSize offsets[1] = { 0 };
				vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &pBatch->GPUBuffer.VertexBuffer->GetVkBuffer(), offsets);
				vkCmdBindIndexBuffer(cmdBuffer, pBatch->GPUBuffer.IndexBuffer->GetVkBuffer(), 0, VK_INDEX_TYPE_UINT32);

				/*	if (!VulkanEngine::GetInstance()->GetVkPhysicalDeviceFeatures().multiDrawIndirect)
					{
						vkCmdDrawIndexedIndirect(cmdBuffer, pBatch->IndirectBuffer->GetVkBuffer(),
							0, pBatch->IndirectCommands.size(), sizeof(VkDrawIndexedIndirectCommand));
					}*/

				for (uint32_t i = 0; i < pBatch->IndirectCommands.size(); ++i)
				{

					/*uint32_t dynamicOffsets[2] = { pBatch->ObjectUboOffset * i ,pBatch->ObjectUboOffset * i };

					vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pCurPipeline->GetVkPipelineLayout(), 1, 1,
						&pBatch->DescriptorSet->GetVkSet(), 2, dynamicOffsets);*/
					uint32_t dynamicOffsets[1] = { pBatch->ObjectUboOffset * i };

					vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pCurPipeline->GetVkPipelineLayout(), 1, 1,
						&pBatch->DescriptorSet->GetVkSet(), 1, dynamicOffsets);
					auto materialDes = pBatch->Objects[i]->MeshPtr->GetMaterial()->GetDescriptorSet()->GetVkSet();
					vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pCurPipeline->GetVkPipelineLayout(), 2, 1,
						&materialDes, 0, nullptr);


					vkCmdDrawIndexedIndirect(cmdBuffer, pBatch->IndirectBuffer->GetVkBuffer(),
						i * sizeof(VkDrawIndexedIndirectCommand), 1, sizeof(VkDrawIndexedIndirectCommand));

				}
				
			}

			vkCmdEndRenderPass(cmdBuffer);
		});

	}
	vkDeviceWaitIdle(VulkanEngine::GetInstance()->GetVkDevice());
}