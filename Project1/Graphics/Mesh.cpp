#include "Mesh.h"

Mesh::Mesh(std::vector<GLfloat>& vboData, std::vector<GLuint>& eboData, GLuint instance, const std::vector<glm::mat4x4>& instanceMatrix, GLenum target, GLenum DrawType) :
	m_vertices{ vboData },
	m_indices{ eboData },
	m_instance{instance},
	m_VAO{},
	m_EBO(eboData),
	m_VBO(vboData, target, DrawType),
	m_instanceVBO(instanceMatrix, target, DrawType)
	
{

	GLsizei stride = 3 * sizeof(GLfloat);

	m_VAO.Bind();

	m_VAO.LinkAttribute(m_VBO, 0, 3, GL_FLOAT, stride, (void*)0); // coordinates
	if (!instanceMatrix.empty())
	{
		GLsizei instanceStride = sizeof(glm::mat4x4);
		//m_VAO.LinkAttribute(m_instanceVBO, 1, 1, GL_FLOAT, sizeof(GLfloat), (void*)(0)); // coordinates
		m_VAO.LinkAttribute(m_instanceVBO, 1, 4, GL_FLOAT, instanceStride, (void*)(0)); // coordinates
		m_VAO.LinkAttribute(m_instanceVBO, 2, 4, GL_FLOAT, instanceStride, (void*)(1 * sizeof(glm::vec4))); // coordinates
		m_VAO.LinkAttribute(m_instanceVBO, 3, 4, GL_FLOAT, instanceStride, (void*)(2 * sizeof(glm::vec4))); // coordinates
		m_VAO.LinkAttribute(m_instanceVBO, 4, 4, GL_FLOAT, instanceStride, (void*)(3 * sizeof(glm::vec4))); // coordinates
		

		glVertexAttribDivisor(1, 1);
		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
	}
	m_indexCount = static_cast<GLsizei>(eboData.size());

	m_VAO.Unbind();
	m_VBO.Unbind();
	
}



Mesh::~Mesh()
{
	m_VAO.Delete();
	m_VBO.Delete();
	m_EBO.Delete();
	m_instanceVBO.Delete();
}

void Mesh::Draw(GLenum mode)
{
	if (m_instance == 1) {
		m_VAO.Bind();
		glDrawElements(mode, m_indexCount, GL_UNSIGNED_INT, 0);
		m_VAO.Unbind();
	}
	else {
		m_VAO.Bind();
		glDrawElementsInstanced(mode, m_indexCount, GL_UNSIGNED_INT, 0, m_instance);
		m_VAO.Unbind();
	}

}
