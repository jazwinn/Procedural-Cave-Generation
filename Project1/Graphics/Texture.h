#pragma once

#include <GL/glew.h>
#include "Shader.h"

class Texture
{

public:
	GLuint m_ID;
	GLenum type;
	Texture(const char* image, GLenum textType, GLenum unitslot, GLenum format, GLenum pixeltype);

	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	void Bind();
	void Unbind();
	void Delete();

	//char* GetData();
};

