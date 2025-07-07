#include "EBO.h"

EBO::EBO(GLuint* indices, GLsizeiptr size, GLenum drawMode)
{
	glGenBuffers(1, &m_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, drawMode);


}

EBO::EBO(const std::vector<GLuint>& indices, GLenum drawMode)
{
	glGenBuffers(1, &m_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), drawMode);
}

void EBO::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
}

void EBO::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::Delete()
{
	glDeleteBuffers(1, &m_ID);
}
