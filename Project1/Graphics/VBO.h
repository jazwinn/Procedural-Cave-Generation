#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class VBO
{
public:

	VBO(const GLfloat* vertices, GLsizeiptr size);

	template <typename T>
	VBO(const std::vector<T>& vertices);

	void Bind();
	void Unbind();
	void Delete();


	GLuint m_ID;

};


template <typename T>
VBO::VBO(const std::vector<T>& vertices)
{
	glGenBuffers(1, &m_ID);//genereate vertex buffer object to store data
	glBindBuffer(GL_ARRAY_BUFFER, m_ID); //bind VBO

	//assign vertices data into the VBO
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), vertices.data(), GL_STATIC_DRAW);
}
