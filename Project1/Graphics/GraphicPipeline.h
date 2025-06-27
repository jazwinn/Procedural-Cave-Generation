#pragma once

#include "window.h"
#include "Shader.h"
#include "Texture.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Shapes.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>


#include <vector>

namespace graphic {
	class GraphicPipeline
	{
	public:
		GraphicPipeline(window::Window& window);
		~GraphicPipeline();
		
		
		void Update(float dt);
		void Draw();

		Camera& Get_Camera() { return m_camera; }
	
	private:

		Shader m_genericShader;
		Shader m_InstancedShader;
		window::Window& m_window;
		Shapes m_Shapes;
		Camera m_camera;
		

	};
}


