#include "Shapes.h"

Shapes::Shapes(Shader& shader):
	m_Shader(shader)
{
	GLfloat vertices[] = {
		 -0.5, -0.5, -0.5 ,
		 -0.5, -0.5, 0.5 ,
		 -0.5, 0.5, -0.5 ,
		 -0.5, 0.5, 0.5 ,
		 0.5, -0.5, -0.5 ,
		 0.5, -0.5, 0.5 ,
		 0.5, 0.5, -0.5 ,
		 0.5, 0.5, 0.5 ,
	};

	GLuint indices[] = {
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

	m_RectangleMesh = std::make_unique<Mesh>(vertices, indices, sizeof(vertices), sizeof(indices));


}

void Shapes::Draw_Rectangle(const glm::mat4& vp, const glm::vec3 center, const glm::vec3 scale, const glm::vec4& color, const DrawType drawtype)
{
	glm::mat4 model = glm::translate(glm::mat4(1.0f), center) * glm::scale(glm::mat4(1.0f), scale);

	glm::mat4 m2w = vp * model;

	m_Shader.setUniform("uniform_m2w", m2w);
	m_Shader.setUniform("uniform_color", color);

	

	switch (drawtype)
	{
	case FILLED:
		m_RectangleMesh->Draw(GL_TRIANGLES);
		break;
	case WIREFRAME:
		glDisable(GL_DEPTH_TEST);
		m_RectangleMesh->Draw(GL_LINES);
		break;
	case FILLEDWIREFRAME:
		m_RectangleMesh->Draw(GL_TRIANGLES);
		m_RectangleMesh->Draw(GL_LINES);
		break;
	default:
		break;
	}
	
	

}
