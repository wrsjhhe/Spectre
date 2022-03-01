#include "Objects/Scene.h"
#include "Materials/Material.h"
#include "Geometries/BufferGeometry.h"
#include "Renderers/Renderer.h"
#include "Timer.h"
#include "GLFWContext.h"
#include "Cameras/PerspectiveCamera.h"
#include "FileUtils.h"
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace Spectre;


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

		pObjectDesc->MateralPtr->VertexShader = FileUtils::ReadFile("../../../../../Resources/Shaders/triangle.vert");
		pObjectDesc->MateralPtr->FragmentShader = FileUtils::ReadFile("../../../../../Resources/Shaders/triangle.frag");

		BufferGeometry* geometry = BufferGeometry::Create(pObjectDesc->VertexAttrs);
		// 顶点数据
		std::vector<float> vertices = {
				1.0f, 1.0f, 0.0f ,     1.0f, 0.0f, 0.0f ,1.0f,
				-1.0f,1.0f, 0.0f ,     0.0f, 1.0f, 0.0f,1.0f,
				 0.0f, -1.0f, 0.0f ,   0.0f, 0.0f, 1.0f,1.0f,
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

		
		PerspectiveCamera camera(DegreesToRadians(75.f), (float)g_Width/ (float)g_Height, 0.1, 3000.0f);
		camera.LookAt({ 0.f,0.f, 2.5f }, { 0.f,0.f, 0.f }, { 0.f,1.f, 0.f });
		renderer.BindCamera(&camera);

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
			fpsCounterSS << "Sample01_Triangle -----";
			fpsCounterSS << std::fixed << std::setprecision(1) << filteredFrameTime * 1000;
			fpsCounterSS << " ms (" << 1.0 / filteredFrameTime << " fps)";
			context.SetTitle(fpsCounterSS.str());

			renderer.Render();

			g_LastTime = currTime;
			g_CurrTime = g_CurrTime + elapsedTime;

			Matrix rMat = Matrix::CreateRotationZ(DegreesToRadians(90.0f) * elapsedTime);
			pMesh->Transform(rMat);
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