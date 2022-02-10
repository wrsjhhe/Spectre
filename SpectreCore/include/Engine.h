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

	void Loop(onEngineLoopCallback loopcb);

	void Exit();

	Engine();

	~Engine();

private:
	RenderSystemVK* m_pRenderSystem = nullptr;
	bool            m_Exit = false;
};

END_NAMESPACE_SPECTRE