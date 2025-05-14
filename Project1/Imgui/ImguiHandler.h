#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include "ImGuizmo.h"

namespace imgui {

	class ImguiHandler
	{
	public:
		ImguiHandler(GLFWwindow* window, const char* glsl_version);
		~ImguiHandler();

		void Update();
		void Render();


	};

}


