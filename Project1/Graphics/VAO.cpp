#include "VAO.h"

VAO::VAO()
{

	glGenVertexArrays(1, &m_ID);
	glBindVertexArray(m_ID);

}

void VAO::LinkAttribute(VBO& VBO, GLuint Layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
	VBO.Bind();
	glVertexAttribPointer(Layout, numComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(Layout);
	VBO.Unbind();

}

void VAO::Bind()
{
	glBindVertexArray(m_ID);

}

void VAO::Unbind()
{
	glBindVertexArray(0);
}

void VAO::Delete()
{
	glDeleteVertexArrays(1, &m_ID);
}
