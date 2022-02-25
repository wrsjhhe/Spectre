#include "Objects/Scene.h"
#include "Materials/Material.h"
#include "Geometries/BufferGeometry.h"
#include "Renderers/Renderer.h"
#include "Timer.h"
#include "GLFWContext.h"
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


int g_Width = 1400;
int g_Height = 900;
double g_CurrTime = 0;
double g_LastTime = 0;

class Sample01_Triangle
{
public:
	void CreateContext(int width, int height)
	{
		context.Create("Sample01_Triangle", width, height);
		context.OnResized = [this](int _width, int _height) {
			if (_width == 0 || _height == 0)
			{
				return;
			}
			renderer.Resize(_width, _height);
		};
	}

	void Run()
	{
		CreateContext(g_Width, g_Height);

		ObjectDesc* pObjectDesc = renderer.CreateObjectDesc();
		pObjectDesc->VertexAttrs = { VertexAttribute_Position, VertexAttribute_Color };

		pObjectDesc->MateralPtr->VertexShader = readFile("../../../../../Resources/Shaders/triangle.vert");
		pObjectDesc->MateralPtr->FragmentShader = readFile("../../../../../Resources/Shaders/triangle.frag");

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


		renderer.Attach({ context.GetWindowHandle() });
		renderer.BindScene(&scene);
		renderer.Resize(g_Width, g_Height);
		renderer.Setup();

		double filteredFrameTime = 0.0;
		double filterScale = 0.2;
		Timer timer;
		g_LastTime = timer.GetElapsedTime();
		while (!context.Closed())
		{
			if (!context.Paused())
			{
				context.WaitEvent();
				continue;		
			}
			context.PollEvent();
			double currTime = timer.GetElapsedTime();
			double elapsedTime = currTime - g_LastTime;

			filteredFrameTime = filteredFrameTime * (1.0 - filterScale) + filterScale * elapsedTime;
			std::stringstream fpsCounterSS;
			fpsCounterSS << std::fixed << std::setprecision(1) << filteredFrameTime * 1000;
			fpsCounterSS << " ms (" << 1.0 / filteredFrameTime << " fps)";
			std::cout << fpsCounterSS.str() << std::endl;

			renderer.Render();
	

			g_LastTime = currTime;
			g_CurrTime = g_CurrTime + elapsedTime;
		}
	}
	
private:
	GLFWContext context;
	GLFWwindow* pWindow;
	Renderer renderer;
};


int main()
{
	Sample01_Triangle app;
	app.Run();

	return 0;
}