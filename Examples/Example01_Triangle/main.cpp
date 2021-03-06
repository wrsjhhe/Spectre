#include "Scenes/Scene.h"
#include "Materials/MeshBasicMaterial.h"
#include "Geometries/BufferGeometry.h"
#include "Renderers/Renderer.h"
#include "Cameras/PerspectiveCamera.h"
#include "ExampleBase.h"
#include "GLFWContext.h"

using namespace Spectre;

int g_Width = 1400;
int g_Height = 900;

class Sample01_Triangle : public ExampleBase
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

		MeshBasicMaterialPtr pMat1 = MeshBasicMaterial::Create();
		pMat1->SetDisplayMode(0);
		pMat1->SetColor({ 84.f ,255.f ,159.f });
		BufferGeometry* geometry1 = BufferGeometry::Create();
		std::vector<Vertex> vertices1 = {
			{{1.0f, 1.0f, 1.0f} },
			{	{-1.0f,1.0f, 1.0f} },
			{{	0.0f, -1.0f, 1.0f} }
		};
		std::vector<uint32_t> indices1 = { 0, 1, 2 };
		geometry1->SetVertices(vertices1.data(), vertices1.size());
		geometry1->SetIndices(indices1.data(), indices1.size());
		MeshPtr pMesh1 = Mesh::Create(geometry1, pMat1);

		MeshBasicMaterialPtr pMat2 = MeshBasicMaterial::Create();
		pMat2->SetDisplayMode(1);
		BufferGeometry* geometry2 = BufferGeometry::Create();
		std::vector<Vertex> vertices2 = {
			{{10.0f, 10.0f, 0.0f} ,    { 255.0f, 0.0f, 0.0f}},
			{	{-10.0f,10.0f, 0.0f} , {  0.0f, 255.0f, 0.0f}},
			{{	0.0f, -10.0f, 0.0f} ,{    0.0f, 0.0f, 255.0f}}
		};
		std::vector<uint32_t> indices2 = { 0, 1, 2 };
		geometry2->SetVertices(vertices2.data(), vertices2.size());
		geometry2->SetIndices(indices2.data(), indices2.size());
		MeshPtr pMesh2 = Mesh::Create(geometry2, pMat2);

		Scene scene;
		scene.AddMesh(pMesh1);
		scene.AddMesh(pMesh2);
		renderer.Attach({ context.GetWindowHandle() });
		renderer.BindScene(&scene);
		renderer.Resize(g_Width, g_Height);

		context.AddKeyEvent(KeyBoard_A, [&](void* window, int action) {
			if (action == 1)
			{
				scene.AddMesh(pMesh2);
			}		
		});

		context.AddKeyEvent(KeyBoard_D, [&](void* window, int action) {
			if (action == 1)
			{
				scene.RemoveMesh(pMesh2->Id());
			}
		});
		
		PerspectiveCamera camera(DegreesToRadians(75.f), (float)g_Width/ (float)g_Height, 0.1, 3000.0f);
		camera.LookAt({ 0.f,0.f, 12.f }, { 0.f,0.f, 0.f }, { 0.f,1.f, 0.f });
		scene.AddCamera(&camera);

		renderer.Setup();

		InitFPS();

		while (!context.Closed())
		{
			if (!context.Paused())
			{
				context.WaitEvent();
				continue;		
			}
			context.PollEvent();

			static std::string fps;
			static double elapsedTime;
			UpdateFPS(fps, elapsedTime);
			context.SetTitle(fps);

			renderer.Render();

			Matrix rMat1 = Matrix::CreateRotationZ(DegreesToRadians(90.0f) * elapsedTime);
			Matrix rMat2 = Matrix::CreateRotationZ(DegreesToRadians(45.0f) * elapsedTime);
			pMesh1->Transform(rMat1);
			pMesh2->Transform(rMat2);
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