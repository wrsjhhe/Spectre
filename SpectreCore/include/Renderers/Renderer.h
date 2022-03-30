#pragma once
#include "SpectreApi.h"
#include "Scenes/Scene.h"
#include "NativeWindow.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapchain.h"
#include "VulkanFrameBuffer.h"

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

	void CreateFrameBuffer();

	void ReadyScene();

	void SetDrawCommand();

private:
	VkSurfaceKHR					  m_Surface = nullptr;

	VulkanSwapChainPtr				  m_SwapChain;
	VulkanRenderPassPtr               m_RenderPass;
	std::vector<VulkanFrameBufferPtr> m_FrameBuffers;
	VulkanImagesPtr					  m_DepthStencilImage;
	VulkanSemaphorePtr				  m_RenderComplete;
	VulkanSemaphorePtr				  m_PresentComplete;

	uint32_t                          m_Width = 0;
	uint32_t                          m_Height = 0;
	bool                              m_Prepared = false;

	Scene*							  m_ScenePtr = nullptr;
	Matrix                            m_MVPData;

};

END_NAMESPACE_SPECTRE