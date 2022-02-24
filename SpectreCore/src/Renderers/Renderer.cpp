#include "Renderers/Renderer.h"

USING_NAMESPACE(Spectre)

Renderer::Renderer()
{

}

void Renderer::Attach(const NativeWindow& wnd)
{

}

void Renderer::BindScene(Scene* pScene)
{
	m_ScenePtr = pScene;
}

void Renderer::BindCamera(Camera* pCamera)
{
	m_CameraPtr = pCamera;
}

void Resize(uint32_t width, uint32_t height)
{

}

void Renderer::Render()
{

}