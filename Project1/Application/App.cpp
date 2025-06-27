#include "App.h"
#include <cmath>
#include <iostream>





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