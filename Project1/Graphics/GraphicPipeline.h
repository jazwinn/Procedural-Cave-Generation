#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "window.h"
#include "Shader.h"
#include "Texture.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Shapes.h"
#include "Camera.h"
#include "Voxel.h"


namespace graphic {
	class GraphicPipeline
	{
	public:
		GraphicPipeline(window::Window& window);
		~GraphicPipeline();
		
		
		void Update(float dt);
		void Draw();
		void DrawImgui();


		Camera& Get_Camera() { return m_camera; }
		Voxel& Get_Voxel() { return m_voxel;}
		Shapes& Get_Shapes() { return m_Shapes; }

	
	private:

		
		Voxel m_voxel;
		Shader m_genericShader;
		Shader m_InstancedShader;
		window::Window& m_window;
		Shapes m_Shapes;
		Camera m_camera;
		

	};
}


