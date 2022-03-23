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
	m_EventHandleMap = { 
		{GLFW_KEY_SPACE,KeyBoard_SPACE} ,

		{GLFW_KEY_A,KeyBoard_A} ,
		{GLFW_KEY_B,KeyBoard_B} ,
		{GLFW_KEY_C,KeyBoard_C} ,
		{GLFW_KEY_D,KeyBoard_D} ,
		{GLFW_KEY_E,KeyBoard_E} ,
		{GLFW_KEY_F,KeyBoard_F} ,
		{GLFW_KEY_G,KeyBoard_G} ,
		{GLFW_KEY_H,KeyBoard_H} ,
		{GLFW_KEY_I,KeyBoard_I} ,
		{GLFW_KEY_J,KeyBoard_J} ,
		{GLFW_KEY_K,KeyBoard_K} ,
		{GLFW_KEY_L,KeyBoard_L} ,
		{GLFW_KEY_M,KeyBoard_M} ,
		{GLFW_KEY_N,KeyBoard_N} ,
		{GLFW_KEY_O,KeyBoard_O} ,
		{GLFW_KEY_P,KeyBoard_P} ,
		{GLFW_KEY_Q,KeyBoard_Q} ,
		{GLFW_KEY_R,KeyBoard_R} ,
		{GLFW_KEY_S,KeyBoard_S} ,
		{GLFW_KEY_T,KeyBoard_T} ,
		{GLFW_KEY_U,KeyBoard_U} ,
		{GLFW_KEY_V,KeyBoard_V} ,
		{GLFW_KEY_W,KeyBoard_W} ,
		{GLFW_KEY_X,KeyBoard_X} ,
		{GLFW_KEY_Y,KeyBoard_Y} ,
		{GLFW_KEY_Z,KeyBoard_Z} ,
						 
		{GLFW_KEY_ESCAPE,KeyBoard_ESCAPE} ,
	};

}
