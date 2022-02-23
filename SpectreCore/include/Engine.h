#pragma once
#include "SpectreDefs.h"
#include "Noncopyable.h"
#include "EngineCreateInfo.h"
#include "Scene.h"

BEGIN_NAMESPACE_SPECTRE

typedef void (*OnEngineLoopCallback)();

typedef void (*OnEngineSleepCallback)();

class RenderSystemVK;
class Engine : public Noncopyable
{

public:
	bool Init(const EngineCreateInfo& info);

	Scene& GetScene() { return m_Scene; }

	void Render();

	void Resize(uint32_t width,uint32_t height);

	void Sleep() { m_Sleep = true; }

	void Awake() { m_Sleep = false; }

	void Exit();

	void SetEngineLoopCallback(OnEngineLoopCallback loopCb) { m_onLoop = loopCb; }
	void SetEngineSleepCallback(OnEngineSleepCallback sleepCb) { m_onSleep = sleepCb; }

	Engine();

	~Engine();

private:
	RenderSystemVK*						m_pRenderSystem = nullptr;
	Scene								m_Scene;
	bool                                m_Sleep = false;

	bool								m_Exit = false;
private:
	OnEngineLoopCallback                m_onLoop;
	OnEngineSleepCallback               m_onSleep;
};

END_NAMESPACE_SPECTRE