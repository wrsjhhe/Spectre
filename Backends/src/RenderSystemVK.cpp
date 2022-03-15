#include "VulkanCommon.h"
#include "VulkanEngine.h"
#include "VulkanContext.h"
#include "VulkanSwapchain.h"
#include "VulkanCommand.h"
#include "VulkanBuffer.h"
#include "VulkanImages.h"
#include "VulkanRenderPass.h"
#include "VulkanFrameBuffer.h"
#include "VulkanSemaphore.h"
#include "VulkanPipeline.h"
#include "VulkanPrimitive.h"
#include "VulkanGui.h"
#include "imgui.h"
#include "RenderSystemVK.h"

USING_NAMESPACE(Spectre)

RenderSystemVK::RenderSystemVK() noexcept
{

	VulkanEngine::VulkanEngineCreateInfo engineCI;
#ifdef VKB_DEBUG
	engineCI.EnableValidation = true;
#endif
	m_VulkanEnginePtr = VulkanEngine::Create(engineCI);

	m_ContextPtr = std::make_shared<VulkanContext>(m_VulkanEnginePtr);
}

RenderSystemVK::~RenderSystemVK()
{
	m_GuiPtr->Destroy();

	for (auto& pPrimitive : m_Primitives)
	{
		delete pPrimitive.second;
		pPrimitive.second = nullptr;
	}
}

void RenderSystemVK::CreateRenderContext(const RenderContextDesc& desc)
{
	m_ContextPtr->InitCommandPool();

	//创建Surface
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hinstance = GetModuleHandle(NULL);
	surfaceCreateInfo.hwnd = (HWND)desc.Window.hWnd;
	VK_CHECK(vkCreateWin32SurfaceKHR(m_VulkanEnginePtr->GetVkInstance(), &surfaceCreateInfo, nullptr, &surface), "Failed create vkSurface!");
	
	uint32_t surafceSupport = 0;
	vkGetPhysicalDeviceSurfaceSupportKHR(m_VulkanEnginePtr->GetVkPhysicalDevice(), m_VulkanEnginePtr->GetGraphicQueue().QueueFamilyIndex, surface, &surafceSupport);
	EXP_CHECK(surafceSupport, "Surface not support");


	m_ContextPtr->CalcSwapchainParamaters(surface);
	
	m_RenderComplete = VulkanSemaphore::CreateSemaphore();
	//m_RenderCommands = VulkanCommand::Create(m_ContextPtr->GetVkGraphicCommandPool(), g_ImageSize);
}


void RenderSystemVK::CreateSwapChain(const SwapChainDesc& desc)
{
	m_Width = desc.Width;
	m_Height = desc.Height;

	m_GuiPtr = std::make_shared<VulkanGui>(m_VulkanEnginePtr);
	m_GuiPtr->Init("Fonts/Ubuntu-Regular.ttf", m_Width,m_Height);

	m_SwapChain = std::make_shared<VulkanSwapChain>(*m_ContextPtr, desc);

	CreateDepthStencil();
	CreateRenderPass();
	CreateFrameBuffer();

}

void RenderSystemVK::RecordCmd(std::function<void(VkCommandBuffer)> cmdFn)
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
	renderPassBeginInfo.renderArea.extent.width = m_SwapChain->GetWidth();
	renderPassBeginInfo.renderArea.extent.height = m_SwapChain->GetHeight();

	auto cmd = VulkanEngine::GetInstance()->GetRenderCmd();
	for (uint32_t i = 0; i < cmd.size(); ++i)
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

		VkDeviceSize offsets[1] = { 0 };

		cmd[i]->RecordCommond([&](VkCommandBuffer cmdBuffer) {
			vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
			vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
			cmdFn(cmdBuffer);
			/*	for (auto kvMesh : m_Primitives)
				{
					kvMesh.second->PipelinePtr->BindDescriptorSets(cmdBuffer);
					kvMesh.second->PipelinePtr->BindPipeline(cmdBuffer);
					kvMesh.second->BindDrawCmd(cmdBuffer);
				}*/
			
			//m_GuiPtr->BindDrawCmd(cmdBuffer, m_RenderPass->GetVkRenderPass());
			vkCmdEndRenderPass(cmdBuffer);		
		});

	}
	vkDeviceWaitIdle(m_VulkanEnginePtr->GetVkDevice());
}

void RenderSystemVK::Draw()
{
	/*m_GuiPtr->StartFrame();

	ImGui::Begin("ImGUI!", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	ImGui::Text("This is some useful text.");
	ImGui::SameLine();

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();

	m_GuiPtr->EndFrame();
	if (m_GuiPtr->Update())
	{
		Setup();
	}*/
	VkDevice device = m_VulkanEnginePtr->GetVkDevice();
	VulkanQueue queue = m_VulkanEnginePtr->GetGraphicQueue();
	int32_t backBufferIndex = m_SwapChain->AcquireImageIndex(m_PresentComplete);
	if (backBufferIndex < 0)
	{
		ReceateSwapchain({ m_Width,m_Height });
		return;
	}

	// 提交绘制命令
	VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	auto cmd = VulkanEngine::GetInstance()->GetRenderCmd();
	cmd[backBufferIndex]->SignalSemaphore = { m_RenderComplete->GetVkSemaphore() };
	cmd[backBufferIndex]->WaitSemaphore = { m_PresentComplete->GetVkSemaphore() };
	cmd[backBufferIndex]->WaitStageMask = { waitStageMask };

	cmd[backBufferIndex]->Submit(queue);
	
	// present
	m_SwapChain->Present(
		queue.VkQueue,
		&m_RenderComplete->GetVkSemaphore()
	);
}

void RenderSystemVK::CreateDepthStencil()
{
	m_DepthStencilImage = VulkanImages::CreateDepthStencilImage(m_Width, m_Height);
}

void RenderSystemVK::CreateRenderPass()
{
	m_RenderPass = VulkanRenderPass::CreateCommonRenderPass(m_ContextPtr->m_VkSwapChainFormat);
}

void RenderSystemVK::CreateFrameBuffer()
{
	uint32_t fwidth = m_SwapChain->GetWidth();
	uint32_t fheight = m_SwapChain->GetHeight();
	VkDevice device = m_VulkanEnginePtr->GetVkDevice();

	std::vector<VkImageView> attachments;
	attachments.resize(2);
	attachments[1] = m_DepthStencilImage->GetVkImageViews()[0];

	const std::vector<VkImageView>& backbufferViews = m_SwapChain->GetImages().GetVkImageViews();

	m_FrameBuffers.resize(backbufferViews.size());
	for (uint32_t i = 0; i < m_FrameBuffers.size(); ++i)
	{
		attachments[0] = backbufferViews[i];
		m_FrameBuffers[i] = VulkanFrameBuffer::CreateFrameBuffer(*m_RenderPass, attachments, m_Width, m_Height);
	}
}


//VulkanPrimitive* RenderSystemVK::AddPrimitive(std::shared_ptr<VulkanPipeline> pPipeline,
//	Vertex* vertices, uint32_t vertCount, uint32_t* indices, uint32_t indCount)
//{
//	VkDevice device = m_VulkanEnginePtr->GetVkDevice();
//	VulkanQueue queue = m_VulkanEnginePtr->GetGraphicQueue();
//
//	auto tempIndexBuffer = VulkanBuffer::Create(indCount * sizeof(uint32_t), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
//	auto tempVertexBuffer = VulkanBuffer::Create(vertCount * sizeof(Vertex), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
//
//	tempIndexBuffer->Map(indices);
//	tempVertexBuffer->Map(vertices);
//
//	VulkanPrimitive* pPrimitive = new VulkanPrimitive();
//	pPrimitive->IndicesBufferPtr = VulkanBuffer::Create(indCount * sizeof(uint32_t), VkBufferUsageFlagBits(VK_BUFFER_USAGE_TRANSFER_DST_BIT| VK_BUFFER_USAGE_INDEX_BUFFER_BIT),
//		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
//	pPrimitive->IndicesCount = indCount;
//	pPrimitive->VertexBufferPtr = VulkanBuffer::Create(vertCount * sizeof(Vertex), VkBufferUsageFlagBits(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT),
//		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
//
//	auto xferCmdBuffer = VulkanEngine::GetInstance()->GetTransformCmd();
//
//	xferCmdBuffer->RecordCommond([&](VkCommandBuffer cmdBuffer) {
//		tempIndexBuffer->CopyTo(pPrimitive->IndicesBufferPtr, cmdBuffer);
//		tempVertexBuffer->CopyTo(pPrimitive->VertexBufferPtr, cmdBuffer);
//	});
//	
//	xferCmdBuffer->Submit(queue);
//
//	pPrimitive->PipelinePtr = pPipeline;
//
//	m_Primitives[pPrimitive->Id()] = pPrimitive;
//
//	return pPrimitive;
//}
void RenderSystemVK::CreatePipeline(VulkanPipelinePtr pipeline)
{
	
}

void RenderSystemVK::UpdateUniformBuffers(void* pBuffe)
{
	for (auto& primKV : m_Primitives)
	{
		auto buffer = primKV.second->PipelinePtr->GetUniformBuffer();
		if (buffer->MapPointerCache!=nullptr)
		{
			std::memcpy(buffer->MapPointerCache, pBuffe, buffer->GetTotalSize());
			buffer->Flush();
		}
		else
		{
			buffer->Map(pBuffe,true);
		}	
	}
}


void RenderSystemVK::ReceateSwapchain(const SwapChainDesc& desc)
{
	SwapChainDesc _desc = desc;
	vkDeviceWaitIdle(m_VulkanEnginePtr->GetVkDevice());

	DestorySwapchain();
	m_ContextPtr->ReCalcSwapchainParamaters();
	m_ContextPtr->CalcSwapChainExtent(_desc.Width, _desc.Height);
	m_Width = _desc.Width;
	m_Height = _desc.Height;

	m_SwapChain = std::make_shared<VulkanSwapChain>(*m_ContextPtr, _desc);
	CreateFrameBuffer();

	//Setup();
}

void RenderSystemVK::DestorySwapchain()
{
	m_FrameBuffers.clear();

	m_SwapChain = nullptr;
}

bool RenderSystemVK::UpdateUI()
{
	m_GuiPtr->StartFrame();

	bool yes = true;
	{
		static float f = 0.0f;
		static Vector3 color(0, 0, 0);
		static int counter = 0;
		ImGuiIO& io = ImGui::GetIO();
		//ImGui::SetNextWindowPos(ImVec2(0, 0));
		//ImGui::SetNextWindowSize(ImVec2(0, 0), ImVec2(io.DisplaySize.x, io.DisplaySize.y));
		ImGui::Begin("ImGUI!", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		ImGui::Text("This is some useful text.");
		ImGui::Checkbox("Demo Window", &yes);
		ImGui::Checkbox("Another Window", &yes);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		ImGui::ColorEdit3("clear color", (float*)&color);

		if (ImGui::Button("Button"))
		{
			counter++;
		}

		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	bool hovered = ImGui::IsItemHovered() || ImGui::IsAnyItemHovered();

	m_GuiPtr->EndFrame();

	//if (m_GuiPtr->Update())
	//{
	//	Setup();
	//}
	return hovered;
}
