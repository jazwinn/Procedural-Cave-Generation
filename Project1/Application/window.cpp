#include "window.h"
#include <iostream>


namespace window {
	Window::Window()
	{
		glfwInit();

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		//create window
		window = glfwCreateWindow(1600, 800, "3D project", NULL, NULL);

		//check if create window is successfull, else stop program
		if (window == NULL) {
			std::cout << "Fail to create window" << std::endl;
			glfwTerminate();
			return;
		}

		// set window to be be current context
		glfwMakeContextCurrent(window);

		//initialize glew
		GLenum err = glewInit();
		if (GLEW_OK != err) {
			std::cerr << "GLEW Error: " << glewGetErrorString(err) << std::endl;
			return;
		}

		//area for open gl to render in (usually the entire window)
		glViewport(0, 0, 1600, 800);

		// set window color to black
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		//swap buffers
		glfwSwapBuffers(window);

	}
	Window::~Window()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}
	int Window::getWindowWidth()
	{
		int width{}, height{};
		glfwGetWindowSize(window, &width, &height);
		return width;
	}
	int Window::getWindowHeigth()
	{
		int width{}, height{};
		glfwGetWindowSize(window, &width, &height);
		return height;
	}
}


