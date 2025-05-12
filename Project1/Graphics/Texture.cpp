#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(const char* image, GLenum textType, GLenum unitslot, GLenum format, GLenum pixeltype)
{
	type = textType;
	int imagewidth, imageheight, numColCh;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(image , &imagewidth, &imageheight, &numColCh, 0);


	glGenTextures(1, &m_ID);
	glActiveTexture(unitslot);
	glBindTexture(textType, m_ID);

	glTextureParameteri(textType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(textType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureParameteri(textType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(textType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(textType, 0, GL_RGBA, imagewidth, imageheight, 0, format, pixeltype, bytes); // RGBA for PNG,  RGB for jpg
	glGenerateMipmap(textType);

	stbi_image_free(bytes);
	glBindTexture(textType, 0);

}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
	shader.Activate();
	shader.setUniform(uniform, unit);//unit : 0 to correspons with GL_TEXTURE0
}

void Texture::Bind()
{
	glBindTexture(type, m_ID);
}

void Texture::Unbind()
{
	glBindTexture(type, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &m_ID);
}

//char* Texture::GetData()
//{
//	GLint width, height, channel, internalFormat;
//
//	// Bind the texture first
//	glBindTexture(GL_TEXTURE_2D, m_ID);
//
//	// Get width and height
//	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
//	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
//	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
//
//	switch (internalFormat) {
//		case GL_RGB:
//			channel = 3;
//			break;
//		case GL_RGBA:
//			channel = 4;
//			break;
//		case GL_RED:
//			channel = 1;
//			break;
//		default:
//			channel = 0;
//			break;
//	}
//
//	//retrieve data
//	char* retrievedData = new char[width * height * internalFormat];
//	glGetTexImage(GL_TEXTURE_2D, 0, format, pixeltype, retrievedData);
//
//
//	return retrievedData;
//}
