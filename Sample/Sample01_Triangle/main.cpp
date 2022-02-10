#include "Engine.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3.h>
#include <glfw3native.h>
using namespace Spectre;

static void OnButton(GLFWwindow* window, int key, int scancode, int action, int mods);
static void onWindowResized(GLFWwindow* window, int width, int height);
class Sample01_Triangle
{
public:
	void Run()
	{
		EngineCreateInfo info;
		info.Width = 1400;
		info.Height = 900;
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		auto window = glfwCreateWindow(info.Width, info.Height, "Vulkan", nullptr, nullptr);
		glfwSetKeyCallback(window, OnButton);

		glfwSetWindowSizeCallback(window, onWindowResized);
		glfwSetWindowUserPointer(window, this);
		void* hwnd = glfwGetWin32Window(window);
		info.Wnd = NativeWindow{ hwnd };


		engine.Init(info);
		engine.Loop([]() {
			glfwPollEvents();
			});

		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void Resize(uint32_t width, uint32_t height)
	{

	}

	void Exit()
	{
		engine.Exit();
	}
private:


private:
	Engine engine;
};


static void OnButton(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Sample01_Triangle* app = reinterpret_cast<Sample01_Triangle*>(glfwGetWindowUserPointer(window));
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		app->Exit();
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

static void onWindowResized(GLFWwindow* window, int width, int height) {
	if (width == 0 || height == 0) return;

	Sample01_Triangle* app = reinterpret_cast<Sample01_Triangle*>(glfwGetWindowUserPointer(window));
	app->Resize(width, height);
}

int main()
{
	Sample01_Triangle app;
	app.Run();

	return 0;
}