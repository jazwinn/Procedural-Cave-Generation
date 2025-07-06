#include "VBO.h"
#include <iostream>

VBO::VBO(const GLfloat* vertices, GLsizeiptr size, GLenum target , GLenum drawMode):
	m_Target(target),
	m_DrawMode(drawMode),
	m_Size(size)
{
	glGenBuffers(1, &m_ID);//genereate vertex buffer object to store data
	glBindBuffer(target, m_ID); //bind VBO

	//assign vertices data into the VBO
	glBufferData(target, size, nullptr, drawMode);
	glBufferSubData(target, 0, size, vertices);
}


void VBO::UpdateData(const void* data, GLsizeiptr size, GLintptr offset)
{
	glBindBuffer(m_Target, m_ID);


	if (size != m_Size) {
		// If the size is different, we need to reallocate the buffer
		glBufferData(m_Target, size, nullptr, m_DrawMode); // Reallocate buffer with new size

		m_Size = size; // Update the size
	}
	glBufferSubData(m_Target, offset, size, data);

}

void VBO::Bind()
{
	glBindBuffer(m_Target, m_ID); //bind VBO
}

void VBO::Unbind()
{
	glBindBuffer(m_Target, 0); //bind VBO
}

void VBO::Delete()
{
	glDeleteBuffers(1, &m_ID);
}
