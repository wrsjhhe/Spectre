#include "Scenes/Scene.h"
#include "Materials/MeshBasicMaterial.h"
#include "Geometries/BufferGeometry.h"
#include "Renderers/Renderer.h"
#include "Timer.h"
#include "GLFWContext.h"
#include "Cameras/PerspectiveCamera.h"
#include <iomanip>



using namespace Spectre;

int g_Width = 1400;
int g_Height = 900;
double g_CurrTime = 0;
double g_LastTime = 0;

class Example02_TextureBox
{
public:
	void CreateContext(int width, int height)
	{
		context.Create("Example02_TextureBox", width, height);
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
		pMat1->SetColor({ 84.f ,255.f ,159.f });
		pMat1->SetTexture("/images/wood.png");
		BufferGeometry* geometry1 = BufferGeometry::Create();
		
		std::vector<Vertex> vertices1 = {
			{{-1,-1,-1},{0,0,0},{0,1} },
			{{-1,+1,-1},{0,0,0},{0,0} },
			{{+1,+1,-1},{0,0,0},{1,0} },
			{{+1,-1,-1},{0,0,0},{1,1} },

			{{-1,-1,-1},{0,0,0},{0,1} },
			{{-1,-1,+1},{0,0,0},{0,0} },
			{{+1,-1,+1},{0,0,0},{1,0} },
			{{+1,-1,-1},{0,0,0},{1,1} },

			{{+1,-1,-1},{0,0,0},{0,1} },
			{{+1,-1,+1},{0,0,0},{1,1} },
			{{+1,+1,+1},{0,0,0},{1,0} },
			{{+1,+1,-1},{0,0,0},{0,0} },

			{{+1,+1,-1},{0,0,0},{0,1} },
			{{+1,+1,+1},{0,0,0},{0,0} },
			{{-1,+1,+1},{0,0,0},{1,0} },
			{{-1,+1,-1},{0,0,0},{1,1} },

			{{-1,+1,-1},{0,0,0},{1,0} },
			{{-1,+1,+1},{0,0,0},{0,0} },
			{{-1,-1,+1},{0,0,0},{0,1} },
			{{-1,-1,-1},{0,0,0},{1,1} },

			{{-1,-1,+1},{0,0,0},{1,1} },
			{{+1,-1,+1},{0,0,0},{0,1} },
			{{+1,+1,+1},{0,0,0},{0,0} },
			{{-1,+1,+1},{0,0,0},{1,0} },
		};
		std::vector<uint32_t> indices1 = { 
				2,0,1,    2,3,0,
				4,6,5,    4,7,6,
				8,10,9,   8,11,10,
				12,14,13, 12,15,14,
				16,18,17, 16,19,18,
				20,21,22, 20,22,23
		};
		geometry1->SetVertices(vertices1.data(), vertices1.size());
		geometry1->SetIndices(indices1.data(), indices1.size());
		MeshPtr pMesh1 = Mesh::Create(geometry1, pMat1);


		Scene scene;
		scene.AddMesh(pMesh1);
		renderer.Attach({ context.GetWindowHandle() });
		renderer.BindScene(&scene);
		renderer.Resize(g_Width, g_Height);


		PerspectiveCamera camera(DegreesToRadians(75.f), (float)g_Width / (float)g_Height, 0.1, 3000.0f);
		camera.LookAt({ 0.f,-5.f, 5.f }, { 0.f,0.f, 0.f }, { 0.f,1.f, 0.f });
		scene.AddCamera(&camera);

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
			fpsCounterSS << "Sample02_TextureBox -----";
			fpsCounterSS << std::fixed << std::setprecision(1) << filteredFrameTime * 1000;
			fpsCounterSS << " ms (" << 1.0 / filteredFrameTime << " fps)";
			context.SetTitle(fpsCounterSS.str());

			renderer.Render();

			g_LastTime = currTime;
			g_CurrTime = g_CurrTime + elapsedTime;

			Matrix rMat1 = Matrix::CreateRotationZ(DegreesToRadians(90.0f) * elapsedTime);
			pMesh1->Transform(rMat1);
		}
	}

private:
	GLFWContext context;
	GLFWwindow* pWindow;
	Renderer renderer;
};


int main()
{
	Example02_TextureBox app;
	app.Run();

	return 0;
}