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

struct UBOData
{
	Matrix MVPMatrix;
};
void Renderer::Setup()
{
	//先统计DescriptorSet的大小，分配一个大的buffer，
	RenderContextDesc renderDesc{};
	renderDesc.Window = m_Window;
	m_pRenderSystem = new RenderSystemVK();
	m_pRenderSystem->CreateRenderContext(renderDesc);

	SwapChainDesc swapChainDesc;
	swapChainDesc.Width = m_Width;
	swapChainDesc.Height = m_Height;
	m_pRenderSystem->CreateSwapChain(swapChainDesc);

	auto meshes = m_ScenePtr->GetMeshes();
	PipelineBufferManager pipelineBufferMgr;
	for (auto* pMesh : meshes)
	{
		BufferMaterialPtr pMaterial = pMesh->GetMaterial();
		std::shared_ptr<VulkanPipeline> pPipeline;
		auto iter =	m_pipelineCache.find(pMaterial->Id());
		if (iter == m_pipelineCache.end())
		{
			PipelineDesc pipelineDesc;
			pipelineDesc.VertexAttributes = pMaterial->GetAttributes();
			pipelineDesc.VertexShaders = { pMaterial->VertexShader };
			pipelineDesc.FragmentShaders = { pMaterial->FragmentShader };
			pipelineDesc.UniformBufferSizes = sizeof(UBOData);
			pPipeline = m_pRenderSystem->CreatePipeline(pipelineDesc);

			PipelineBufferManagerPtr bufferMsg = std::make_shared<PipelineBufferManager>();
			m_pipelineCache[pMaterial->Id()] = { pPipeline,bufferMsg };
		}
		else
		{
			//pPipeline = iter->second;
		}

		m_pRenderSystem->AddPrimitive(pPipeline,
			pMesh->GetBufferGeometry()->Vertices(), pMesh->GetBufferGeometry()->VerticesCount(),
			pMesh->GetBufferGeometry()->Indices(), pMesh->GetBufferGeometry()->IndicesCount());
	}

	m_pRenderSystem->Setup();

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
