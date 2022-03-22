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
	typedef std::function<void(int, int)> WindowResizedCallbackFunc;
	typedef std::function<void(void*)> InputCallbackFunc;

	WindowResizedCallbackFunc OnResized = nullptr;
	//InputCallbackFunc OnInputed;
public:
	GLFWContext();
	~GLFWContext();

	bool Paused() const;

	void Create(const char* name, int width, int height);

	void SetTitle(const std::string& title);

	void AddKeyEvent(KeyBoardType key, InputCallbackFunc func);

	void GetWindowSize(int* width,int* height);

	const std::unordered_map<KeyBoardType, std::vector<InputCallbackFunc>>& GetKeyEvents() const { return m_Events; }

	void* GetWindowHandle();

	void PollEvent();

	void WaitEvent();

	void Close();

	bool Closed();


private:

#if defined(SPE_PLATFORM_WINDOWS)
	HWND GetWin32Window();
#endif

	void InitEventMap();
private:
	GLFWwindow* m_Window = nullptr;
	std::map<int, KeyBoardType> m_EventHandleMap;
	std::unordered_map<KeyBoardType, std::vector<InputCallbackFunc>> m_Events;
};