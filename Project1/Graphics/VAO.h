#pragma once


#include "VBO.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class VAO
{
public:
	
	VAO();
	
	/**
	 * @brief Links a vertex buffer object (VBO) to a vertex attribute in OpenGL.
	 *
	 * This function specifies how OpenGL should interpret the data stored in the VBO
	 * when using vertex attributes in a shader program.
	 *
	 * @param VBO The vertex buffer object to link.
	 * @param Layout The index of the vertex attribute (location in the shader).
	 * @param numComponents The number of components per vertex attribute (e.g., 3 for vec3).
	 * @param type The data type of the components (e.g., GL_FLOAT).
	 * @param stride The byte offset between consecutive vertex attributes.
	 * @param offset A pointer to the offset in the buffer where the attribute data begins.
	 */
	void LinkAttribute(VBO& VBO, GLuint Layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset );
	void Bind();
	void Unbind();
	void Delete();
	
	
	GLuint m_ID;



};

