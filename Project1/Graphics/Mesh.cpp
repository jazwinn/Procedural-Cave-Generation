#include "Mesh.h"

Mesh::Mesh(GLfloat* vboData, GLuint* eboData, GLuint vboSize, GLuint eboSize):
	m_VAO{},
	m_VBO(vboData, vboSize),
	m_EBO(eboData, eboSize)
{

	GLsizei stride = 3 * sizeof(GLfloat);

	m_VAO.Bind();

	m_VAO.LinkAttribute(m_VBO, 0, 3, GL_FLOAT, stride, (void*)0); // coordinates

	m_vertexCount = vboSize / stride;
	m_indexCount = eboSize / sizeof(GLuint);

	m_VAO.Unbind();
	m_VBO.Unbind();
	
}


Mesh::~Mesh()
{
	m_VAO.Delete();
	m_VBO.Delete();
}

void Mesh::Draw(GLenum mode)
{
	m_VAO.Bind();
	glDrawElements(mode, m_indexCount, GL_UNSIGNED_INT, 0);
	m_VAO.Unbind();
}
