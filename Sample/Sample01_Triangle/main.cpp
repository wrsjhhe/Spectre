#include "Objects/Scene.h"
#include "Materials/Material.h"
#include "Geometries/BufferGeometry.h"
#include "Renderers/Renderer.h"
#include "Timer.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3.h>
#include <glfw3native.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace Spectre;

static std::string readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}
	size_t fileSize = (size_t)file.tellg();
	std::string buffer;
	buffer.resize(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

static void OnButton(GLFWwindow* window, int key, int scancode, int action, int mods);
static void onWindowResized(GLFWwindow* window, int width, int height);

int g_Width = 1400;
int g_Height = 900;
double g_CurrTime = 0;
double g_LastTime = 0;

class Sample01_Triangle
{
public:
	NativeWindow CreateWnd(int width,int height)
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		pWindow = glfwCreateWindow(width, height, "Sample01_Triangle", nullptr, nullptr);
		glfwSetKeyCallback(pWindow, OnButton);

		glfwSetWindowSizeCallback(pWindow, onWindowResized);
		glfwSetWindowUserPointer(pWindow, this);
		void* hwnd = glfwGetWin32Window(pWindow);
		return NativeWindow{ hwnd };
	}

	void Run()
	{

		ObjectDesc* pObjectDesc = renderer.CreateObjectDesc();
		pObjectDesc->VertexAttrs = { VertexAttribute_Position, VertexAttribute_Color };

		pObjectDesc->MateralPtr->VertexShader = readFile("../../../../../../Resources/Shaders/triangle.vert");
		pObjectDesc->MateralPtr->FragmentShader = readFile("../../../../../../Resources/Shaders/triangle.frag");

		BufferGeometry* geometry = BufferGeometry::Create(pObjectDesc->VertexAttrs);
		// 顶点数据
		std::vector<float> vertices = {
				1.0f, 1.0f, 0.0f ,1.0f, 0.0f, 0.0f ,1.0f,
				-1.0f,1.0f, 0.0f , 0.0f, 1.0f, 0.0f,1.0f,
				 0.0f, -1.0f, 0.0f , 0.0f, 0.0f, 1.0f,1.0f,
		};

		// 索引数据
		std::vector<uint32_t> indices = { 0, 1, 2 };
		geometry->SetVertices(vertices.data(), vertices.size());
		geometry->SetFaceIndex(indices.data(), indices.size());

		Mesh* pMesh = Mesh::Create(geometry, pObjectDesc->MateralPtr);

		Scene scene;
		scene.Add(pMesh);

		auto wnd = CreateWnd(g_Width, g_Height);

		renderer.Attach(wnd);
		renderer.BindScene(&scene);
		renderer.Resize(g_Width, g_Height);
		renderer.Setup();

		double filteredFrameTime = 0.0;
		double filterScale = 0.2;
		Timer timer;
		g_LastTime = timer.GetElapsedTime();
		while (!exit)
		{
			if (sleep)
			{
				glfwWaitEvents();
				continue;		
			}
			double currTime = timer.GetElapsedTime();
			double elapsedTime = currTime - g_LastTime;

			filteredFrameTime = filteredFrameTime * (1.0 - filterScale) + filterScale * elapsedTime;
			std::stringstream fpsCounterSS;
			fpsCounterSS << std::fixed << std::setprecision(1) << filteredFrameTime * 1000;
			fpsCounterSS << " ms (" << 1.0 / filteredFrameTime << " fps)";
			std::cout << fpsCounterSS.str() << std::endl;

			Present();
			glfwPollEvents();

			g_LastTime = currTime;
			g_CurrTime = g_CurrTime + elapsedTime;
		}
		glfwDestroyWindow(pWindow);
		glfwTerminate();
	}

	void Exit()
	{
		exit = true;
	}

	void Present()
	{
		renderer.Render();
	}

	void Sleep()
	{
		sleep = true;
	}

	void Awake()
	{
		sleep = false;
	}
	
	Renderer& GetRnderer() { return renderer; }
private:
	GLFWwindow* pWindow;
	Renderer renderer;
	bool exit = false;
	bool sleep = false;
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

	Sample01_Triangle* app = reinterpret_cast<Sample01_Triangle*>(glfwGetWindowUserPointer(window));

	if (width == 0 || height == 0)
	{
		app->Sleep();
	}
	else
	{
		app->Awake();
		app->GetRnderer().Resize(width, height);
	}
}

int main()
{
	Sample01_Triangle app;
	app.Run();

	return 0;
}