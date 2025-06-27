#include "Shapes.h"

Shapes::Shapes(Shader& shader):
	m_Shader(shader)
{

	std::vector<GLfloat> vertices =
	{
		-0.5, -0.5, -0.5 ,
		 -0.5, -0.5, 0.5 ,
		 -0.5, 0.5, -0.5 ,
		 -0.5, 0.5, 0.5 ,
		 0.5, -0.5, -0.5 ,
		 0.5, -0.5, 0.5 ,
		 0.5, 0.5, -0.5 ,
		 0.5, 0.5, 0.5 ,
	};

	std::vector<GLuint> indices = {
		0, 1, 
		0, 2, 
		0, 4, 
		3, 2, 
		3, 1,  
		3, 7,  
		5, 4, 
		5, 7,  
		5, 1, 
		6, 7, 
		6, 4,  
		6, 2   
	};



	m_RectangleMesh = std::make_unique<Mesh>(vertices, indices);

	std::vector<Instance> transforms;

	const int gridSize = 10;
	float scaleFactor = 0.1f; // Scale factor for the cubes

	glm::vec3 scale = glm::vec3(scaleFactor); // Scale for each cube
	glm::vec3 center = glm::vec3(0.f, 0.f, 0.f); // Center of the grid
	glm::vec3 offset = - glm::vec3(gridSize / 2.f) + glm::vec3(scaleFactor/2.f);

	for (float x = 0.f; x < gridSize; x += scaleFactor) {
		for (float y = 0.f; y < gridSize; y += scaleFactor) {
			for (float z = 0.f; z < gridSize; z += scaleFactor) {
				
				glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z) + offset) * glm::scale(glm::mat4(1.0f), scale);
				transforms.emplace_back(model, 1.f);
			}
		}
	}


	m_InstancedCube = std::make_unique<Mesh>(vertices, indices, transforms.size(), transforms);
}

void Shapes::Draw_Rectangle(const glm::mat4& vp, const glm::vec3 center, const glm::vec3 scale, const glm::vec4& color, const DrawType drawtype, const glm::vec4& wireFrameColor )
{
	glm::mat4 model = glm::translate(glm::mat4(1.0f), center) * glm::scale(glm::mat4(1.0f), scale);

	glm::mat4 m2w = vp * model;

	m_Shader.setUniform("uniform_m2w", m2w);
	m_Shader.setUniform("uniform_color", color);

	

	switch (drawtype)
	{
	case FILLED:
		m_RectangleMesh->Draw(GL_TRIANGLE_FAN);
		break;
	case WIREFRAME:
		glDisable(GL_DEPTH_TEST);
		m_RectangleMesh->Draw(GL_LINES);
		break;
	case FILLEDWIREFRAME:
		m_RectangleMesh->Draw(GL_TRIANGLE_FAN);

		m_Shader.setUniform("uniform_color", wireFrameColor);
		m_RectangleMesh->Draw(GL_LINES);
		break;
	default:
		break;
	}
	
	

}

void Shapes::Draw_InstancedCubes(const glm::mat4& vp, const glm::vec4& color, DrawType drawtype, const glm::vec4& wireFrameColor)
{

	m_Shader.setUniform("uniform_m2w", vp);
	m_Shader.setUniform("uniform_color", color);

	switch (drawtype)
	{
	case FILLED:
		m_InstancedCube->Draw(GL_TRIANGLE_FAN);
		break;
	case WIREFRAME:
		glDisable(GL_DEPTH_TEST);
		m_InstancedCube->Draw(GL_LINES);
		break;
	case FILLEDWIREFRAME:
		m_InstancedCube->Draw(GL_TRIANGLE_FAN);

		m_Shader.setUniform("uniform_color", wireFrameColor);
		m_InstancedCube->Draw(GL_LINES);
		break;
	default:
		break;
	}

}
