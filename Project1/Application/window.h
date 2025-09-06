#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace window {
	class Window {
	public:
		Window();
		~Window();

		int GetWindowWidth();
		int GetWindowHeigth();



		GLFWwindow* window;
	};
}