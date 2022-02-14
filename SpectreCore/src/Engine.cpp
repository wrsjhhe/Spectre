#include "Vulkan/RenderSystemVK.h"
#include "Vulkan/VulkanGraphicTypes.h"
#include "Engine.h"

#include "Geometry/Vertex.h"


USING_NAMESPACE(Spectre)

bool Engine::Init(const EngineCreateInfo& info)
{
	m_pRenderSystem = new RenderSystemVK();
	m_pRenderSystem->CreateRenderContext();

	m_pRenderSystem->CreateSurface(info.Wnd);

	SwapChainDesc swapChainDesc;
	swapChainDesc.Width = info.Width;
	swapChainDesc.Height = info.Height;
	m_pRenderSystem->CreateSwapChain(swapChainDesc);
	return true;
}

void Engine::Render()
{
	auto meshes = m_Scene.GetRootNode()->TraverseMeshes();
	
	uint32_t recordVertexIndex = 0;
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	for (auto& mesh : meshes)
	{
		uint32_t curVerticesSize =	vertices.size();
		uint32_t* pFace = mesh->Faces();
		for (uint32_t i = 0;i < mesh->FacesCount();++i)
		{
			indices.emplace_back(*pFace + vertices.size());
			++pFace;
		}

		Vertex* pVertex = mesh->Vertices();
		for (uint32_t i = 0; i < mesh->FacesCount(); ++i)
		{
			vertices.emplace_back(*pVertex);
			++pVertex;
		}
		recordVertexIndex += vertices.size();
	}
	m_pRenderSystem->CreateMeshBuffers(vertices, indices);

	m_pRenderSystem->Setup();
	while (!m_Exit)
	{
		if (m_Sleep && m_onSleep!=nullptr)
		{
			m_onSleep();
			continue;
		}
		if (m_onLoop != nullptr)
			m_onLoop();
		m_pRenderSystem->Draw();
	}
}

void Engine::Resize(uint32_t width, uint32_t height)
{
	SwapChainDesc swapChainDesc;
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	m_pRenderSystem->ReceateSwapchain(swapChainDesc);
}

void Engine::Exit()
{
	m_Exit = true;
}

Engine::Engine()
{

}

Engine::~Engine()
{
	delete m_pRenderSystem;
}
