#include "App.h"
#include <cmath>
#include <iostream>

void CreateHeartShape(std::shared_ptr<Chunks> chunk)
{
	if (!chunk) return;

	int w = chunk->GetWidth();
	int h = chunk->GetHeight();
	int d = chunk->GetDepth();

	// Center coordinates in the chunk
	float cx = w / 2.0f;
	float cy = h / 2.0f;
	float cz = d / 2.0f;

	// Scale factors to map block indices to [-1.5,1.5] range roughly
	float scaleX = 3.0f / w;
	float scaleY = 3.0f / h;
	float scaleZ = 3.0f / d;
	
	for (int z = 0; z < d; ++z) {
		for (int y = 0; y < h; ++y) {
			for (int x = 0; x < w; ++x) {
				// Map to [-1.5, 1.5] space centered at zero
				float nx = (x - cx) * scaleX;
				float ny = (y - cy) * scaleY;
				float nz = (z - cz) * scaleZ;

				// Heart formula:
				float lhs = pow(nx * nx + (9.0f / 4.0f) * ny * ny + nz * nz - 1, 3);
				float rhs = nx * nx * pow(nz, 3) + (9.0f / 80.0f) * ny * ny * pow(nz, 3);

				if (lhs - rhs <= 0) {
					chunk->at(x, y, z) = SOLID;
				}
				else {
					chunk->at(x, y, z) = EMPTY;
				}
			}
		}
	}
}



namespace app {
	App::App() :
		m_window{}, 
		m_graphicPipeline(m_window), 
		m_imgui(m_window.window, "#version 130")
	{

	}


	App::~App()
	{

	}
	int App::Run()
	{

		Voxel& voxel = m_graphicPipeline.Get_Voxel();
		int key = voxel.AddChunk(0, 0, 0, 32, 32, 32, 0.1);


		std::shared_ptr<Chunks> chunk = voxel.GetChunk(key);

		//CreateHeartShape(chunk);
		chunk->at(0, 0, 0) = EMPTY;

		voxel.UpdateChunk(key);

		//note to draw inside while loop
		Shapes& shape = m_graphicPipeline.Get_Shapes();
		Camera& camera = m_graphicPipeline.Get_Camera();

		




		float lastTime = static_cast<float>(glfwGetTime());
		float deltaTime = 0.0f;

		while (!glfwWindowShouldClose(m_window.window)) {
			float currentTime = static_cast<float>(glfwGetTime());
			deltaTime = currentTime - lastTime;
			lastTime = currentTime;

			glfwPollEvents();
			glClearDepth(1.0);
			glClearColor(0.f, 0.f, 0.f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// UPDATE
			m_imgui.Update();
			m_graphicPipeline.Update(deltaTime);
			

			// DRAW
			m_graphicPipeline.Draw();
			m_imgui.Render();

			glfwSwapBuffers(m_window.window);
		}



		return 0;
	}
}