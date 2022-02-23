#include "Engine.h"
#include "ShaderTool.h"
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

		auto window = glfwCreateWindow(info.Width, info.Height, "Sample01_Triangle", nullptr, nullptr);
		glfwSetKeyCallback(window, OnButton);

		glfwSetWindowSizeCallback(window, onWindowResized);
		glfwSetWindowUserPointer(window, this);
		void* hwnd = glfwGetWin32Window(window);
		info.Wnd = NativeWindow{ hwnd };

		std::string  strVertexShader = readFile("../../../../../../Resources/Shaders/triangle.vert");
		std::string  strFragmentShader = readFile("../../../../../../Resources/Shaders/triangle.frag");
		std::vector<uint32_t> vertexSPV = ShaderTool::Compile_glsl(strVertexShader, ShaderType_Vertex);
		std::vector<uint32_t> fragmentSPV = ShaderTool::Compile_glsl(strFragmentShader, ShaderType_Fragment);
		info.VertexShaders.emplace_back(std::move(vertexSPV));
		info.FragmentShaders.emplace_back(std::move(fragmentSPV));

		engine.Init(info);

		// 顶点数据
		std::vector<float> vertices = {
				1.0f, 1.0f, 0.0f ,1.0f, 0.0f, 0.0f ,
				-1.0f,1.0f, 0.0f , 0.0f, 1.0f, 0.0f,
				 0.0f, -1.0f, 0.0f , 0.0f, 0.0f, 1.0f
		};

		// 索引数据
		std::vector<uint32_t> indices = { 0, 1, 2 };

		Mesh* pMesh = new Mesh();

		pMesh->SetVertices(vertices.data(), vertices.size());
		pMesh->SetFaceIndex(indices.data(), indices.size());

		engine.GetScene().GetRootNode()->AddMesh(pMesh);

		double filteredFrameTime = 0.0;
		engine.SetEngineLoopCallback([&](double currTime, double elapsedTime) {
			glfwPollEvents();
			double filterScale = 0.2;
			filteredFrameTime = filteredFrameTime * (1.0 - filterScale) + filterScale * elapsedTime;
			std::stringstream fpsCounterSS;
			fpsCounterSS << std::fixed << std::setprecision(1) << filteredFrameTime * 1000;
			fpsCounterSS << " ms (" << 1.0 / filteredFrameTime << " fps)";
			std::cout << fpsCounterSS.str() << std::endl;
			});
		engine.SetEngineSleepCallback([]() {
			glfwWaitEvents();
			});
		engine.Render();

		glfwDestroyWindow(window);
		glfwTerminate();
	}
	
	Engine& GetEngine() { return engine; }
private:
	Engine engine;
};


static void OnButton(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Sample01_Triangle* app = reinterpret_cast<Sample01_Triangle*>(glfwGetWindowUserPointer(window));
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		app->GetEngine().Exit();
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

static void onWindowResized(GLFWwindow* window, int width, int height) {

	Sample01_Triangle* app = reinterpret_cast<Sample01_Triangle*>(glfwGetWindowUserPointer(window));

	if (width == 0 || height == 0)
	{
		app->GetEngine().Sleep();
	}
	else
	{
		app->GetEngine().Awake();
		app->GetEngine().Resize(width, height);
	}
}

int main()
{
	Sample01_Triangle app;
	app.Run();

	return 0;
}