#pragma once
#include "SpectreDefs.h"
#include "Objects/Scene.h"
#include "Cameras/Camera.h"
#include "NativeWindow.h"
BEGIN_NAMESPACE_SPECTRE

class Renderer
{
public:
	Renderer();

	void Attach(const NativeWindow& wnd);

	void BindScene(Scene* pScene);

	void BindCamera(Camera* pCamera);

	void Resize(uint32_t width, uint32_t height);

	void Render();
private:
	Scene* m_ScenePtr;
	Camera* m_CameraPtr;
};

END_NAMESPACE_SPECTRE