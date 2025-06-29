#include "Shapes.h"

Shapes::Shapes(Shader& shader, Shader& instancedShader):
	m_Shader(shader),
	m_InstancedShader(instancedShader)
{

	std::vector<GLfloat> verticesRect =
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

	std::vector<GLuint> indicesRect = {
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



	m_RectangleMesh = std::make_unique<Mesh>(verticesRect, indicesRect);

	std::vector<GLfloat> verticesQuad = {
		-0.5f, -0.5f, 0.0f,  
		 0.5f, -0.5f, 0.0f,  
		 0.5f,  0.5f, 0.0f,  
		-0.5f,  0.5f, 0.0f   
	};


	std::vector<GLuint> indicesQuad = {
    0, 1, 2, 
	2, 3, 0 
	};

	m_QuadMesh = std::make_unique<Mesh>(verticesQuad, indicesQuad);

	std::vector<GLfloat> lineVertices = {
	0.0f, 0.0f, 0.0f,  
	1.0f, 1.0f, 1.0f   
	};

	std::vector<GLuint> lineIndices = {
		0, 1
	};

	m_LineMesh = std::make_unique<Mesh>(lineVertices, lineIndices);

}

void Shapes::Draw_Rectangle(const glm::mat4& vp, const glm::vec3 center, const glm::vec3 scale, const glm::vec4& color, const DrawType drawtype, const glm::vec4& wireFrameColor )
{
	m_Shader.Activate();
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

void Shapes::Draw_Quad(const glm::mat4& vp, const glm::vec3 center, const glm::vec3 scale, const glm::vec4& color, DrawType drawtype, const glm::vec4& wireFrameColor)
{
	m_Shader.Activate();
	glm::mat4 model = glm::translate(glm::mat4(1.0f), center) * glm::scale(glm::mat4(1.0f), scale);

	glm::mat4 m2w = vp * model;

	m_Shader.setUniform("uniform_m2w", m2w);
	m_Shader.setUniform("uniform_color", color);


	switch (drawtype)
	{
	case FILLED:

		m_QuadMesh->Draw(GL_TRIANGLES);
		break;
	case WIREFRAME:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		m_QuadMesh->Draw(GL_LINES);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		break;
	case FILLEDWIREFRAME:
		m_QuadMesh->Draw(GL_TRIANGLES);

		m_Shader.setUniform("uniform_color", wireFrameColor);
		m_QuadMesh->Draw(GL_LINES);
		break;
	default:
		break;
	}
}

void Shapes::Draw_Line(const glm::mat4& vp, const glm::vec3 start, const glm::vec3 end, const glm::vec4& color)
{
	m_Shader.Activate();
	glm::mat4 model = glm::translate(glm::mat4(1.f), start) * glm::scale(glm::mat4(1.f), end - start);

	glm::mat4 m2w = vp * model;	

	m_Shader.setUniform("uniform_m2w", m2w);
	m_Shader.setUniform("uniform_color", color);

	m_LineMesh->Draw(GL_LINES);

}

void Shapes::Draw_InstancedCubes(const glm::mat4& vp, const glm::vec4& color, DrawType drawtype, const glm::vec4& wireFrameColor)
{
	m_InstancedShader.Activate();
	m_InstancedShader.setUniform("uniform_m2w", vp);
	m_InstancedShader.setUniform("uniform_color", color);

	switch (drawtype)
	{
	case FILLED:
		m_InstancedCube->Draw(GL_TRIANGLE_FAN);
		break;
	case WIREFRAME:
	
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

void Shapes::Draw_InstancedQuads(const glm::mat4& vp, const glm::vec4& color, DrawType drawtype, const glm::vec4& wireFrameColor)
{
	m_InstancedShader.Activate();
	m_InstancedShader.setUniform("uniform_m2w", vp);
	m_InstancedShader.setUniform("uniform_color", color);

	switch (drawtype)
	{
	case FILLED:

		m_InstancedQuad->Draw(GL_TRIANGLES);
		break;
	case WIREFRAME:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		m_InstancedQuad->Draw(GL_TRIANGLES);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		break;
	case FILLEDWIREFRAME:
		m_InstancedQuad->Draw(GL_TRIANGLES);

		m_Shader.setUniform("uniform_color", wireFrameColor);
		m_InstancedQuad->Draw(GL_LINES);
		break;
	default:
		break;
	}
}
