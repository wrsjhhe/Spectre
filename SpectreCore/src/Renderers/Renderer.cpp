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

	
	m_ScenePtr->PrepareStageBuffer();
	m_pRenderSystem->CreatePipeline(m_ScenePtr->TestPipeline);
	m_ScenePtr->RefreshGPUBuffer();
	//auto meshes = m_ScenePtr->GetMeshes();
	//for (auto* pMesh : meshes)
	//{
	//	BufferMaterialPtr pMaterial = pMesh->GetMaterial();
	//	PipelineDesc pipelineDesc;
	//	pipelineDesc.VertexAttributes = pMaterial->GetAttributes();
	//	pipelineDesc.VertexShaders = { pMaterial->GetVertexShader() };
	//	pipelineDesc.FragmentShaders = { pMaterial->GetFragmentShader() };
	//	pipelineDesc.UniformBufferSizes = sizeof(UBOData);
	//	m_ScenePtr->TestPipeline = m_pRenderSystem->CreatePipeline(pipelineDesc);


	//	//m_pRenderSystem->AddPrimitive(pPipeline,
	//	//	pMesh->GetBufferGeometry()->Vertices(), pMesh->GetBufferGeometry()->VerticesCount(),
	//	//	pMesh->GetBufferGeometry()->Indices(), pMesh->GetBufferGeometry()->IndicesCount());
	//}

	m_pRenderSystem->RecordCmd([this](VkCommandBuffer cmdBuffer)
	{
		m_ScenePtr->TestPipeline->BindDescriptorSets(cmdBuffer);
		m_ScenePtr->TestPipeline->BindPipeline(cmdBuffer);
		VkDeviceSize offsets[1] = { 0 };
	/*	vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &VertexBufferPtr->GetVkBuffer(), offsets);
		vkCmdBindIndexBuffer(cmdBuffer, IndicesBufferPtr->GetVkBuffer(), 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(cmdBuffer, IndicesCount, 1, 0, 0, 0);*/
	});

	m_Prepared = true;

}

void Renderer::Render()
{
	m_MVPData = m_PerspectiveCameraPtr->GetViewProjection();
	for (auto* pMesh : m_ScenePtr->GetMeshes())
	{
		m_MVPData = pMesh->GetTransformMatrix() * m_MVPData;
	}

	m_pRenderSystem->UpdateUniformBuffers(&m_MVPData);
	m_pRenderSystem->Draw();
}
