#include "RenderSystemVK.h"
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
		m_pRenderSystem->ReceateSwapchain(swapChainDesc);
	}
}


void Renderer::Setup()
{
	RenderContextDesc renderDesc{};
	renderDesc.Window = m_Window;
	m_pRenderSystem = new RenderSystemVK();
	m_pRenderSystem->CreateRenderContext(renderDesc);

	SwapChainDesc swapChainDesc;
	swapChainDesc.Width = m_Width;
	swapChainDesc.Height = m_Height;
	m_pRenderSystem->CreateSwapChain(swapChainDesc);

	m_ScenePtr->CreatePipeline();
	m_pRenderSystem->CreatePipeline(m_ScenePtr->TestPipeline);
	m_ScenePtr->PrepareStageBuffer();
	m_ScenePtr->RefreshGPUBuffer();


	m_pRenderSystem->RecordCmd([this](VkCommandBuffer cmdBuffer)
	{
		m_ScenePtr->TestPipeline->BindDescriptorSets(cmdBuffer);
		m_ScenePtr->TestPipeline->BindPipeline(cmdBuffer);
		VkDeviceSize offsets[1] = { 0 };

		const std::vector<RenderObject>& psssObjs = m_ScenePtr->GetPassObjects();
		vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &m_ScenePtr->m_MergedVertexBuffer.GetDeviceBuffer()->GetVkBuffer(), offsets);
		vkCmdBindIndexBuffer(cmdBuffer, m_ScenePtr->m_MergedIndexBuffer.GetDeviceBuffer()->GetVkBuffer(), 0, VK_INDEX_TYPE_UINT32);
		//vkCmdDrawIndexed(cmdBuffer, 3, 1, 0, 0, 0);
		for (uint32_t i = 0; i < m_ScenePtr->TestIndirectCommands.size(); ++i)
		{
			auto indirectCmd = m_ScenePtr->TestIndirectCommands[i];
			vkCmdDrawIndexedIndirect(cmdBuffer, m_ScenePtr->TestIndirectBuffer->GetVkBuffer(),
				i * sizeof(VkDrawIndexedIndirectCommand), 1, sizeof(VkDrawIndexedIndirectCommand));

		}
	});

	m_Prepared = true;

}

void Renderer::Render()
{
	m_MVPData = m_PerspectiveCameraPtr->GetViewProjection();

	for (auto& obj : m_ScenePtr->GetPassObjects())
	{
		auto m = obj.MeshPtr->GetTransformMatrix();
		m_MVPData = m * m_MVPData;
	}
	auto& buffer = m_ScenePtr->TestPipeline->GetUniformBuffer();
    auto s = buffer->GetTotalSize();
	if (buffer->MapPointerCache != nullptr)
	{
		std::memcpy(buffer->MapPointerCache, &m_MVPData, buffer->GetTotalSize());
		buffer->Flush();
	}
	else
	{
		buffer->Map(&m_MVPData, buffer->GetTotalSize(),0,true);
	}

	m_pRenderSystem->Draw();
}
