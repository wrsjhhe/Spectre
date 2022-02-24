#include "Vulkan/RenderSystemVK.h"
#include "RenderContextDesc.h"
#include "Renderers/Renderer.h"

USING_NAMESPACE(Spectre)

Renderer::Renderer()
{


}

void Renderer::Attach(const NativeWindow& wnd)
{
	m_Window = wnd;
}

ObjectDesc* Renderer::CreateObjectDesc()
{
	ObjectDesc* pDesc = new ObjectDesc;
	pDesc->MateralPtr = new Material;
	m_ObjectDecs.emplace_back(pDesc);
	return pDesc;
}

void Renderer::BindScene(Scene* pScene)
{
	m_ScenePtr = pScene;
}

void Renderer::BindCamera(Camera* pCamera)
{
	m_CameraPtr = pCamera;
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
	renderDesc.VertexAttrs = m_ObjectDecs[0]->VertexAttrs;
	m_ObjectDecs[0]->MateralPtr->CompileSpv();
	renderDesc.VertexShaders = { m_ObjectDecs[0]->MateralPtr->GetVertexSPV() };
	renderDesc.FragmentShaders = { m_ObjectDecs[0]->MateralPtr->GetFragmentSPV() };

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

	m_pRenderSystem->Setup();

	m_Prepared = true;

}

void Renderer::Render()
{
	m_pRenderSystem->Draw();
}

void Renderer::CreatePipeline()
{

}
