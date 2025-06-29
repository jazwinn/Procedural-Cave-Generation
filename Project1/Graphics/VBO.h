#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class VBO
{
public:

	VBO(const GLfloat* vertices, GLsizeiptr size, GLenum target = GL_ARRAY_BUFFER, GLenum drawMode = GL_STATIC_DRAW);

	template <typename T>
	VBO(const std::vector<T>& vertices, GLenum target = GL_ARRAY_BUFFER, GLenum drawMode = GL_STATIC_DRAW);

	void UpdateData(const void* data, GLsizeiptr size, GLintptr offset);

	void Bind();
	void Unbind();
	void Delete();

private:
	GLuint m_ID;
	GLenum m_Target;
	GLenum m_DrawMode;
	GLuint m_Size;
};


template <typename T>
VBO::VBO(const std::vector<T>& vertices, GLenum target, GLenum drawMode):
	m_Target(target),
	m_DrawMode(drawMode),
	m_Size(vertices.size() * sizeof(T))
{
	glGenBuffers(1, &m_ID);//genereate vertex buffer object to store data
	glBindBuffer(target, m_ID); //bind VBO

	//assign vertices data into the VBO
	glBufferData(target, m_Size, nullptr, drawMode);
	glBufferSubData(target, 0, m_Size, vertices.data());

}
