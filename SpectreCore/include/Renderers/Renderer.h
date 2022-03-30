#pragma once
#include "SpectreApi.h"
#include "Scenes/Scene.h"
#include "NativeWindow.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapchain.h"

BEGIN_NAMESPACE_SPECTRE



class RenderSystemVK;
class VulkanPipeline;
class Renderer :public SpectreApi
{
public:
	DefineClass(Renderer);
	Renderer();
	~Renderer();
	void Attach(const NativeWindow& wnd);

	void BindScene(Scene* pScene);

	//void BindCamera(PerspectiveCamera* pCamera);

	void Resize(uint32_t width, uint32_t height);

	void Setup();

	void Render();

private:
	void CreateRenderPass();

	void CreateSwapchain();

	void ReadyScene();

	void SetDrawCommandFunc();

	void SetDrawCommand();

private:
	RenderSystemVK*					  m_pRenderSystem = nullptr;
	VkSurfaceKHR					  m_Surface = nullptr;

	VulkanSwapChainPtr				  m_SwapChain;
	VulkanRenderPassPtr               m_RenderPass;

	uint32_t                          m_Width = 0;
	uint32_t                          m_Height = 0;
	bool                              m_Prepared = false;

	Scene*							  m_ScenePtr = nullptr;
	Matrix                            m_MVPData;

};

END_NAMESPACE_SPECTRE