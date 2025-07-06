#include "ImguiHandler.h"
#include <iostream>
#include <fstream>


imgui::ImguiHandler::ImguiHandler(GLFWwindow* window, const char* glsl_version)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::SetCurrentContext(ImGui::GetCurrentContext()); // Ensure context is set

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

}

imgui::ImguiHandler::~ImguiHandler()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


}

void imgui::ImguiHandler::Update()
{
	// new frame
	ImGui_ImplOpenGL3_NewFrame(); //needs to be called every frame
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();


	//ImGui::Begin("Window");







	//ImGui::End();

}

void imgui::ImguiHandler::Render()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Render();


	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}
