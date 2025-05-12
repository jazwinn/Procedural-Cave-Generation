#include "GraphicPipeline.h"


#include <algorithm>

namespace graphic{
	


	GraphicPipeline::GraphicPipeline()
	{

		//// for test
		//GLfloat vertices[] =
		//{ //     COORDINATES     /        COLORS      /   TexCoord  //
		//	-0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,	0.0f, 0.0f, // Lower left corner
		//	-0.5f,  0.5f, 0.0f,     0.0f, 1.0f, 0.0f,	0.0f, 1.0f, // Upper left corner
		//	 0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,	1.0f, 1.0f, // Upper right corner
		//	 0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,	1.0f, 0.0f  // Lower right corner
		//};

		//// Indices for vertices order
		//GLuint indices[] =
		//{
		//	0, 2, 1, // Upper triangle
		//	0, 3, 2 // Lower triangle
		//};

		//Shader shaderprogram("./Graphics/Shaders/genericVertexShader.vert", "./Graphics/Shaders/genericFragmentShader.frag");

		//VAO VAO1;
		//VAO1.Bind();

		//VBO VBO1(vertices, sizeof(vertices));
		//EBO EBO1(indices, sizeof(indices));

		//VAO1.LinkAttribute(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(GLfloat), (void*)0);
		//VAO1.LinkAttribute(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
		//VAO1.LinkAttribute(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));


		//VAO1.Unbind();
		//VBO1.Unbind();
		//EBO1.Unbind();


		//vec_EBO.emplace_back(EBO1);
		//vec_VAO.emplace_back(VAO1);
		//vec_VBO.emplace_back(VBO1);
		//vec_Shader.emplace_back(shaderprogram);

		//Texture rombatex("./Assets/Texture/image.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
		//rombatex.texUnit(shaderprogram, "tex0", 0);

		//vec_Texture.emplace_back(rombatex);


		

	}

	GraphicPipeline::~GraphicPipeline()
	{
		//std::for_each(vec_VAO.begin(), vec_VAO.end(), [](auto x) { x.Delete(); });
		//std::for_each(vec_VBO.begin(), vec_VBO.end(), [](auto x) { x.Delete(); });
		//std::for_each(vec_EBO.begin(), vec_EBO.end(), [](auto x) { x.Delete(); });
		//std::for_each(vec_Texture.begin(), vec_Texture.end(), [](auto x) { x.Delete(); });
		//std::for_each(vec_Shader.begin(), vec_Shader.end(), [](auto x) { x.Delete(); });
	}

	void GraphicPipeline::Draw()
	{

		//vec_Shader[0].Activate();

		//vec_Shader[0].setUniform("scale", 0.5f);

		//vec_Texture[0].Bind();
		//vec_VAO[0].Bind();

		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // 9 refering to number of indices

		

	}

}