#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class EBO
{
public:
	EBO() = default;

	EBO(GLuint* indices, GLsizeiptr size);

	void Bind();
	void Unbind();
	void Delete();


	GLuint m_ID;

};
