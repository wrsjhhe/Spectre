#pragma once
#include "SpectreDefs.h"
#include "Objects/Scene.h"
#include "Cameras/Camera.h"
#include "Materials/Material.h"
#include "NativeWindow.h"

BEGIN_NAMESPACE_SPECTRE

struct ObjectDesc
{
	Material* MateralPtr;
	std::vector<VertexAttribute> VertexAttrs;
};

class RenderSystemVK;
class Renderer
{
public:
	Renderer();

	std::string Id() { return "1"; }

	void Attach(const NativeWindow& wnd);

	ObjectDesc* CreateObjectDesc();

	void BindScene(Scene* pScene);

	void BindCamera(Camera* pCamera);

	void Resize(uint32_t width, uint32_t height);

	void Setup();

	void Render();

private:
	void CreatePipeline();

private:
	RenderSystemVK* m_pRenderSystem = nullptr;

	uint32_t                          m_Width = 0;
	uint32_t                          m_Height = 0;
	bool                              m_Prepared = false;
	NativeWindow					  m_Window;
	Scene*							  m_ScenePtr = nullptr;
	Camera*							  m_CameraPtr = nullptr;
	std::vector<ObjectDesc*>		  m_ObjectDecs;
};

END_NAMESPACE_SPECTRE