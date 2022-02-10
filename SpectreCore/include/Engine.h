#pragma once
#include "CommonDefinitions.h"
#include "Noncopyable.h"
#include "EngineCreateInfo.h"
BEGIN_NAMESPACE_SPECTRE

typedef void (*onEngineLoopCallback)();

class RenderSystemVK;
class Engine : public Noncopyable
{
public:
	bool Init(const EngineCreateInfo& info);

	void Loop(onEngineLoopCallback loopCb);

	void Resize(uint32_t width,uint32_t height);

	void Exit();

	Engine();

	~Engine();

private:
	RenderSystemVK* m_pRenderSystem = nullptr;
	bool            m_Exit = false;
	NativeWindow	m_window;
};

END_NAMESPACE_SPECTRE