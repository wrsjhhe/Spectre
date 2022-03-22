#include "Scenes/Scene.h"
#include "Materials/BufferMaterial.h"
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

		std::vector<VertexAttribute> VertexAttrs = { VertexAttribute_Position, VertexAttribute_Color };

		BufferMaterialPtr pMat = BufferMaterial::Create();
		pMat->SetVertexShader(FileUtils::ReadFile("Shaders/triangle.vert")) ;
		pMat->SetFragmentShader(FileUtils::ReadFile("Shaders/triangle.frag"));
		pMat->DefineAttributes(VertexAttrs);
		BufferGeometry* geometry1 = BufferGeometry::Create(VertexAttrs);
		// 顶点数据
		std::vector<Vertex> vertices1 = {
			{{1.0f, 1.0f, 1.0f} ,    { 1.0f, 0.0f, 0.0f}},
			{	{-1.0f,1.0f, 1.0f} , {    0.0f, 1.0f, 0.0f}},
			{{	0.0f, -1.0f, 1.0f} ,{    0.0f, 0.0f, 1.0f}}
		};

		// 索引数据
		std::vector<uint32_t> indices1 = { 0, 1, 2 };
		geometry1->SetVertices(vertices1.data(), vertices1.size());
		geometry1->SetFaceIndex(indices1.data(), indices1.size());

		MeshPtr pMesh1 = Mesh::Create(geometry1, pMat);

		BufferGeometry* geometry2 = BufferGeometry::Create(VertexAttrs);
		// 顶点数据
		std::vector<Vertex> vertices2 = {
			{{10.0f, 10.0f, 0.0f} ,    { 1.0f, 0.0f, 0.0f}},
			{	{-10.0f,10.0f, 0.0f} , {    0.0f, 1.0f, 0.0f}},
			{{	0.0f, -10.0f, 0.0f} ,{    0.0f, 0.0f, 1.0f}}
		};

		// 索引数据
		std::vector<uint32_t> indices2 = { 0, 1, 2 };
		geometry2->SetVertices(vertices2.data(), vertices2.size());
		geometry2->SetFaceIndex(indices2.data(), indices2.size());

		MeshPtr pMesh2 = Mesh::Create(geometry2, pMat);

		Scene scene;
		scene.AddMesh(pMesh1);
		//scene.AddMesh(pMesh2);
		renderer.Attach({ context.GetWindowHandle() });
		renderer.BindScene(&scene);
		renderer.Resize(g_Width, g_Height);

		context.AddKeyEvent(KeyBoard_SPACE, [&](void* window) {
			scene.AddMesh(pMesh2);
		});
		
		PerspectiveCamera camera(DegreesToRadians(75.f), (float)g_Width/ (float)g_Height, 0.1, 3000.0f);
		camera.LookAt({ 0.f,0.f, 12.f }, { 0.f,0.f, 0.f }, { 0.f,1.f, 0.f });
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
			pMesh1->Transform(rMat);
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