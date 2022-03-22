#include "GLFWContext.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3.h>
#include <glfw3native.h>


GLFWContext::GLFWContext()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	InitEventMap();
}

GLFWContext::~GLFWContext()
{

}

bool GLFWContext::Paused() const
{
	int width = 0, height = 0;
	glfwGetWindowSize(m_Window, &width, &height);
	if (width == 0 || height == 0)
	{
		return false;
	}

	return true;
}



void GLFWContext::Create(const char* name, int width, int height)
{
	m_Window = glfwCreateWindow(width, height, name, nullptr, nullptr);
	glfwSetWindowUserPointer(m_Window, this);

	glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
		GLFWContext* pUser = reinterpret_cast<GLFWContext*>(glfwGetWindowUserPointer(window));

		if (pUser->OnResized != nullptr)
		{
			pUser->OnResized(width,height);
		}

	});
	glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		GLFWContext* pUser = reinterpret_cast<GLFWContext*>(glfwGetWindowUserPointer(window));

		auto keyIter = pUser->m_EventHandleMap.find(key);
		if (keyIter == pUser->m_EventHandleMap.end())
		{
			return;
		}

		KeyBoardType type = pUser->m_EventHandleMap[key];
		auto events = pUser->GetKeyEvents();
		auto iter = events.find(type);
		if (iter != events.end())
		{
			for(auto& ev : iter->second)
			{
				ev(window, action);
			}
		}
	

		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			pUser->Close();
		}
	});

	glfwSetWindowCloseCallback(m_Window,[](GLFWwindow* window) {
		GLFWContext* pUser = reinterpret_cast<GLFWContext*>(glfwGetWindowUserPointer(window));
		pUser->Close();
		return;
	});

	AddKeyEvent(KeyBoard_ESCAPE, [this](void* window,int action) {
		glfwSetWindowShouldClose((GLFWwindow*)window, GLFW_TRUE);
		Close();
	});
}

void GLFWContext::SetTitle(const std::string& title)
{
	glfwSetWindowTitle(m_Window,title.c_str());
}

void GLFWContext::AddKeyEvent(KeyBoardType key, InputCallbackFunc func)
{
	m_Events[key].push_back(func);

}

void GLFWContext::GetWindowSize(int* width, int* height)
{
	glfwGetWindowSize(m_Window, width, height);
}

void* GLFWContext::GetWindowHandle()
{
	return (void*)GetWin32Window();
}

void GLFWContext::PollEvent()
{
	glfwPollEvents();
}

void GLFWContext::WaitEvent()
{
	glfwWaitEvents();
}

void GLFWContext::Close()
{
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
	glfwDestroyWindow(m_Window);
	glfwTerminate();

	m_Window = nullptr;
}

bool GLFWContext::Closed()
{
	return m_Window == nullptr;
}

#if defined(SPE_PLATFORM_WINDOWS)
HWND GLFWContext::GetWin32Window()
{
	return glfwGetWin32Window(m_Window);
}
#endif


void GLFWContext::InitEventMap()
{
	m_EventHandleMap[GLFW_KEY_ESCAPE] = KeyBoard_ESCAPE;
	m_EventHandleMap[GLFW_KEY_SPACE] = KeyBoard_SPACE;
}
