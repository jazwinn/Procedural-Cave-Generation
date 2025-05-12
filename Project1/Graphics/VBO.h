#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class VBO
{
public:

	VBO(GLfloat* vertices, GLsizeiptr size);

	void Bind();
	void Unbind();
	void Delete();


	GLuint m_ID;

};

