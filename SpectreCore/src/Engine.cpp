#include "Vulkan/RenderSystemVK.h"
#include "Vulkan/VulkanGraphicTypes.h"
#include "Engine.h"

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

void Engine::Render(onEngineLoopCallback loopCB)
{
	auto meshes = m_Scene.GetRootNode()->TraverseMeshes();
	
	for (auto& mesh : meshes)
	{

	}


	m_pRenderSystem->Setup();
	while (!m_Exit)
	{
		if (loopCB != nullptr)
			loopCB();
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
