#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

class EBO
{
public:
	EBO() = default;

	EBO(GLuint* indices, GLsizeiptr size, GLenum drawMode = GL_STATIC_DRAW);
	EBO(const std::vector<GLuint>& indices, GLenum drawMode = GL_STATIC_DRAW);

	void Bind();
	void Unbind();
	void Delete();


	GLuint m_ID;

};
