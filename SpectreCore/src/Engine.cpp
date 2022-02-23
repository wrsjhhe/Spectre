#include "Vulkan/RenderSystemVK.h"
#include "RenderDefs.h"
#include "Engine.h"

#include "RenderContextDesc.h"
#include "Timer.h"
USING_NAMESPACE(Spectre)

double g_CurrTime = 0;
double g_LastTime = 0;

bool Engine::Init(const EngineCreateInfo& info)
{
	RenderContextDesc renderDesc{};
	renderDesc.Window = info.Wnd;
	renderDesc.VertexAttrs = { VertexAttribute_Position, VertexAttribute_Color};
	renderDesc.VertexShaders = info.VertexShaders;
	renderDesc.FragmentShaders = info.FragmentShaders;

	m_pRenderSystem = new RenderSystemVK();
	m_pRenderSystem->CreateRenderContext(renderDesc);

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
	std::vector<float> vertices;
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

		float* pVertex = mesh->Vertices();
		for (uint32_t i = 0; i < mesh->VerticesCount(); ++i)
		{
			vertices.emplace_back(*pVertex);
			++pVertex;
		}
		recordVertexIndex += vertices.size();
	}
	m_pRenderSystem->CreateMeshBuffers(vertices, indices);

	m_pRenderSystem->Setup();

	Timer Timer;
	g_LastTime = Timer.GetElapsedTime();
	while (!m_Exit)
	{
		double currTime = Timer.GetElapsedTime();
		double elapsedTime = currTime - g_LastTime;
	
		if (m_Sleep && m_onSleep!=nullptr)
		{
			m_onSleep();
			continue;
		}
		if (m_onLoop != nullptr)
			m_onLoop(g_CurrTime, elapsedTime);
		m_pRenderSystem->Draw();

		g_LastTime = currTime;
		g_CurrTime = g_CurrTime + elapsedTime;
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
