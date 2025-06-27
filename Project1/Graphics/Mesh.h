#pragma once

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"


struct Instance {
	
	glm::mat4 modelMatrix;
	GLfloat visable = 0.f;
};


class Mesh
{
public:
	Mesh(std::vector<GLfloat>& vboData, std::vector<GLuint>& eboData, GLuint instanced = 1, const std::vector<Instance>& instanceMatrix = {});

	~Mesh();
	void Draw(GLenum mode);
private:
	VAO m_VAO;
	VBO m_VBO;
	VBO m_instanceVBO;
	EBO m_EBO;

	GLuint m_vertexCount;
	GLuint m_indexCount;

	GLuint m_instance;

	std::vector<GLfloat> m_vertices;
	std::vector<GLuint> m_indices;
};

