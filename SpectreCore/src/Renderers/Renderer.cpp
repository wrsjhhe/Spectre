#include "Vulkan/RenderSystemVK.h"
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
	renderDesc.VertexAttrs = m_ScenePtr->GetMeshes()[0]->GetBufferGeometry()->VertexAttributes();
	MeshBasicMaterialPtr pMaterial = m_ScenePtr->GetMeshes()[0]->GetMaterial();
	pMaterial->CompileSpv();
	renderDesc.VertexShaders = { pMaterial->GetVertexSPV() };
	renderDesc.FragmentShaders = { pMaterial->GetFragmentSPV() };

	m_pRenderSystem = new RenderSystemVK();
	m_pRenderSystem->CreateRenderContext(renderDesc);

	SwapChainDesc swapChainDesc;
	swapChainDesc.Width = m_Width;
	swapChainDesc.Height = m_Height;
	m_pRenderSystem->CreateSwapChain(swapChainDesc);

	auto& meshes = m_ScenePtr->GetMeshes();

	uint32_t recordVertexIndex = 0;
	std::vector<float> vertices;
	std::vector<uint32_t> indices;


	for (auto& mesh : meshes)
	{
		uint32_t curVerticesSize = vertices.size();
		uint32_t* pFace = mesh->GetBufferGeometry()->Indices();
		for (uint32_t i = 0; i < mesh->GetBufferGeometry()->IndicesCount(); ++i)
		{
			indices.emplace_back(*pFace + vertices.size());
			++pFace;
		}

		float* pVertex = mesh->GetBufferGeometry()->Vertices();
		for (uint32_t i = 0; i < mesh->GetBufferGeometry()->VerticesCount(); ++i)
		{
			vertices.emplace_back(*pVertex);
			++pVertex;
		}
		recordVertexIndex += vertices.size();
	}
	m_pRenderSystem->CreateMeshBuffers(vertices, indices);

	m_MVPData = m_PerspectiveCameraPtr->GetViewProjection();

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

	m_pRenderSystem->UpdateUniformBuffers(m_MVPData);
	m_pRenderSystem->Draw();
}

void Renderer::CreatePipeline()
{

}
