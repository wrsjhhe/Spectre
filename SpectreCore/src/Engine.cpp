#include "Vulkan/RenderSystemVK.h"
#include "Vulkan/VulkanGraphicTypes.h"
#include "Engine.h"

USING_NAMESPACE(Spectre)

bool Engine::Init(const EngineCreateInfo& info)
{
	m_pRenderSystem = new RenderSystemVK();
	m_pRenderSystem->CreateRenderContext();

	SwapChainDesc swapChainDesc;
	swapChainDesc.Width = info.Width;
	swapChainDesc.Height = info.Height;
	m_pRenderSystem->CreateSwapChain(info.Wnd, swapChainDesc);
	return true;
}

void Engine::Loop(onEngineLoopCallback loopcb)
{
	m_pRenderSystem->Setup();
	while (!m_Exit)
	{
		if (loopcb != nullptr)
			loopcb();
		m_pRenderSystem->Draw();
	}
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
