#include "VBO.h"

VBO::VBO(const GLfloat* vertices, GLsizeiptr size)
{
	glGenBuffers(1, &m_ID);//genereate vertex buffer object to store data
	glBindBuffer(GL_ARRAY_BUFFER, m_ID); //bind VBO

	//assign vertices data into the VBO
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}


void VBO::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_ID); //bind VBO
}

void VBO::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0); //bind VBO
}

void VBO::Delete()
{
	glDeleteBuffers(1, &m_ID);
}
