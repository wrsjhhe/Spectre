#pragma once
#include "SpectreApi.h"
#include "Scenes/Scene.h"
#include "Cameras/PerspectiveCamera.h"
#include "Materials/BufferMaterial.h"
#include "NativeWindow.h"
#include "VulkanPipeline.h"

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

	void BindCamera(PerspectiveCamera* pCamera);

	void Resize(uint32_t width, uint32_t height);

	void Setup();

	void Render();

private:
	void ReadyScene();

	void SetDrawCommandFunc();

private:
	RenderSystemVK* m_pRenderSystem = nullptr;

	uint32_t                          m_Width = 0;
	uint32_t                          m_Height = 0;
	bool                              m_Prepared = false;
	NativeWindow					  m_Window;
	Scene*							  m_ScenePtr = nullptr;
	BufferMaterialPtr				  m_MaterialPtr = nullptr;
	PerspectiveCamera*				  m_PerspectiveCameraPtr = nullptr;
	Matrix                            m_MVPData;
};

END_NAMESPACE_SPECTRE