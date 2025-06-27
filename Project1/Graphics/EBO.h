#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

class EBO
{
public:
	EBO() = default;

	EBO(GLuint* indices, GLsizeiptr size);
	EBO(const std::vector<GLuint>& indices);

	void Bind();
	void Unbind();
	void Delete();


	GLuint m_ID;

};
