#include "GraphicPipeline.h"
#include <stb_image.h>
#include "imgui.h"
#include <algorithm>

namespace graphic { 
	
	void sizeCallback(GLFWwindow* pWindow, int width, int height)
	{
		int fbWidth, fbHeight;
		glfwGetFramebufferSize(pWindow, &fbWidth, &fbHeight);
		glViewport(0, 0, fbWidth, fbHeight);
		auto* pipeline = static_cast<GraphicPipeline*>(glfwGetWindowUserPointer(pWindow));
		if (pipeline) {
			pipeline->Get_Camera().Resize(width, height);

		glScissor(0, 0, width, height);
		}
	}

	GraphicPipeline::GraphicPipeline(window::Window& _window):
		m_genericShader("./Graphics/Shaders/genericVertexShader.vert", "./Graphics/Shaders/genericFragmentShader.frag"),
		m_InstancedShader("./Graphics/Shaders/instancedVertexShader.vert", "./Graphics/Shaders/instancedFragmentShader.frag"),
		m_window{ _window },
		m_Shapes{m_genericShader, m_InstancedShader},
		m_voxel{ m_InstancedShader , m_genericShader},
		m_camera(m_window.getWindowWidth(), m_window.getWindowHeigth())
	{
		//Callbacks
		glfwSetWindowUserPointer(m_window.window, this);
		glfwSetWindowSizeCallback(m_window.window, sizeCallback);

	}

	GraphicPipeline::~GraphicPipeline()
	{
		m_genericShader.Delete();
	}

	void GraphicPipeline::Update(float dt)
	{
		int display_w = 0;
		int display_h = 0;
		glfwGetFramebufferSize(m_window.window, &display_w, &display_h);

		double cursor_x = 0.0;
		double cursor_y = 0.0;
		glfwGetCursorPos(m_window.window, &cursor_x, &cursor_y);

		if (glfwGetMouseButton(m_window.window, GLFW_MOUSE_BUTTON_2) != 0) {
			glm::vec3 side = glm::normalize(glm::cross(m_camera.direction, { 0, 1, 0 }));
			glm::vec3 up = glm::normalize(glm::cross(m_camera.direction, side));
			glm::vec3 forward = glm::normalize(m_camera.direction);


			glm::vec3 movement(0.0f);

			float speed = 80.0f;
			if (glfwGetKey(m_window.window, GLFW_KEY_W) == GLFW_PRESS) movement += forward;
			if (glfwGetKey(m_window.window, GLFW_KEY_S) == GLFW_PRESS) movement -= forward;
			if (glfwGetKey(m_window.window, GLFW_KEY_D) == GLFW_PRESS) movement += side;
			if (glfwGetKey(m_window.window, GLFW_KEY_A) == GLFW_PRESS) movement -= side;
			if (glfwGetKey(m_window.window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) movement += up;
			if (glfwGetKey(m_window.window, GLFW_KEY_SPACE) == GLFW_PRESS) movement -= up;

			if (glm::length(movement) > 0.0f) {
				m_camera.SetPosition(m_camera.GetPosition() + glm::normalize(movement) * speed * dt);
			}
				

			glm::vec2 cursor_delta = { (float)cursor_x - m_camera.prevMouseCursor.x, (float)cursor_y - m_camera.prevMouseCursor.y };
			float     rotation_speed = 0.01f;
			m_camera.direction = glm::vec3(glm::vec4(m_camera.direction, 0) * glm::rotate(glm::mat4(1.0f), cursor_delta.y * rotation_speed, side));
			m_camera.direction = glm::vec3(glm::vec4(m_camera.direction, 0) * glm::rotate(glm::mat4(1.0f), cursor_delta.x * rotation_speed, glm::vec3(0, 1, 0)));

		}

		m_camera.UpdateProjection();
		m_camera.UpdateView();
		m_camera.UpdateViewProjection();
		m_camera.prevMouseCursor = glm::vec2{cursor_x, cursor_y};
	}



	void GraphicPipeline::Draw() {
		glEnable(GL_SCISSOR_TEST);
		glEnable(GL_DEPTH_TEST);

		if (config.backFaceCulling) {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);
		}
		else {
			glDisable(GL_CULL_FACE);
		}


		//INSTANCED
		m_voxel.DrawVoxel(m_camera.GetViewProjectionMatrix(), m_voxel.color, GL_LINES);


	}

	void GraphicPipeline::DrawImgui()
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::CollapsingHeader("Graphics Setting")) {
			ImGui::Checkbox("Greedy", &m_voxel.config.greedy);
			if(ImGui::Checkbox("Smooth", &m_voxel.config.marchingCube)) {
				if (m_voxel.config.marchingCube == true) {
					config.backFaceCulling = false;
				}
			}
			ImGui::ColorEdit4("Voxel Color", &m_voxel.color.x);
			ImGui::Separator();
			ImGui::Checkbox("Back Face Culling", &config.backFaceCulling);
		}
	}

}