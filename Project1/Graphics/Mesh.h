#pragma once

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

class Mesh
{
public:
	Mesh(GLfloat* vboData, GLuint* eboData, GLuint vboSize, GLuint eboSize);

	~Mesh();
	void Draw(GLenum mode);
private:
	VAO m_VAO;
	VBO m_VBO;
	EBO m_EBO;
	GLuint m_vertexCount;
	GLuint m_indexCount;
};

