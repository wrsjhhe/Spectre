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

std::vector<Vertex> vertices = {
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
std::vector<uint32_t> indices = {
		2,0,1,    2,3,0,
		4,6,5,    4,7,6,
		8,10,9,   8,11,10,
		12,14,13, 12,15,14,
		16,18,17, 16,19,18,
		20,21,22, 20,22,23
};

class Example02_TextureBox : public ExampleBase
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

		MeshBasicMaterialPtr pMatertal1 = MeshBasicMaterial::Create();
		pMatertal1->SetColor({ 84.f ,255.f ,159.f });
		pMatertal1->SetTexture("/images/wood.png");
		BufferGeometry* pGeometry1 = BufferGeometry::Create();		
		pGeometry1->SetVertices(vertices.data(), vertices.size());
		pGeometry1->SetIndices(indices.data(), indices.size());
		MeshPtr pMesh1 = Mesh::Create(pGeometry1, pMatertal1);

		MeshBasicMaterialPtr pMatertal2 = MeshBasicMaterial::Create();
		pMatertal2->SetColor({ 84.f ,255.f ,159.f });
		pMatertal2->SetTexture("/images/wood.png");
		BufferGeometry* pGeometry2 = BufferGeometry::Create();
		pGeometry2->SetVertices(vertices.data(), vertices.size());
		pGeometry2->SetIndices(indices.data(), indices.size());
		MeshPtr pMesh2 = Mesh::Create(pGeometry2, pMatertal2);
		Matrix tMat = Matrix::CreateTranslation({ 5.f,0.f,0.f });
		pMesh2->Transform(tMat);

		Scene scene;
		scene.AddMesh(pMesh1);
		scene.AddMesh(pMesh2);
		renderer.Attach({ context.GetWindowHandle() });
		renderer.BindScene(&scene);
		renderer.Resize(g_Width, g_Height);


		PerspectiveCamera camera(DegreesToRadians(75.f), (float)g_Width / (float)g_Height, 0.1, 3000.0f);
		camera.LookAt({ 0.f,-5.f, 15.f }, { 0.f,0.f, 0.f });
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

			Matrix rMat = Matrix::CreateRotationZ(DegreesToRadians(90.0f) * elapsedTime);
			pMesh1->Transform(rMat);

			Matrix tMat = Matrix::CreateTranslation({ -5.f,0.f,0.f });
			rMat = Matrix::CreateRotationY(DegreesToRadians(90.0f) * elapsedTime);
			pMesh2->Transform(tMat);
			pMesh2->Transform(rMat);
			tMat = Matrix::CreateTranslation({ 5.f,0.f,0.f });
			pMesh2->Transform(tMat);

			renderer.Render();

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