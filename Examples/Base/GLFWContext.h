#pragma once

#include <functional>
#include <map>
#include "Platform.h"
#include "InputType.h"
#if defined(SPE_PLATFORM_WINDOWS)
#include <windows.h>
#endif

class GLFWwindow;
class GLFWContext
{
public:
	GLFWContext();
	~GLFWContext();

	bool Paused() const;

	void Create(const char* name, int width, int height);

	void GetWindowSize(int* width,int* height);

	void* GetWindowHandle();

	void PollEvent();

	void WaitEvent();

	void Close();

	bool Closed();
public:
	typedef std::function<void(int, int)> WindowResizedCallbackFunc;
	typedef std::function<void(KeyBoardType)> InputCallbackFunc;

	WindowResizedCallbackFunc OnResized = nullptr;
	InputCallbackFunc OnInputed;

private:

#if defined(SPE_PLATFORM_WINDOWS)
	HWND GetWin32Window();
#endif

	void InitEventMap();
private:
	GLFWwindow* m_Window = nullptr;
	std::map<int, KeyBoardType> m_EventMap;
};