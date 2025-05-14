#include "App.h"
#include <cmath>
#include <iostream>
#include <stb_image.h>
#include <glm/gtc/matrix_transform.hpp>



namespace app {
	App::App() :m_window{}, m_graphicPipeline{}, m_imgui(m_window.m_window, "#version 130")
	{

	}
	App::~App()
	{

	}
	int App::Run()
	{
		Shader shaderprogram("./Graphics/Shaders/genericVertexShader.vert", "./Graphics/Shaders/genericFragmentShader.frag");


		int width, height, nChannels;
		unsigned char* data = stbi_load("./Assets/Texture/iceland_heightmap.png",
			&width, &height, &nChannels,
			0);

		if (!data) {
			std::cout << "Fail to load height map" << std::endl;
		}

		std::vector<float> vertices;
		float yScale = 64.0f / 256.0f, yShift = 16.0f;  // apply a scale+shift to the height data
		for (unsigned int i = 0; i < height; i++)
		{
			for (unsigned int j = 0; j < width; j++)
			{
				// retrieve texel for (i,j) tex coord
				unsigned char* texel = data + (j + width * i) * nChannels;
				// raw height at coordinate
				unsigned char y = texel[0];

				// vertex
				vertices.push_back(-height / 2.0f + i);        // v.x
				vertices.push_back((int)y * yScale - yShift); // v.y
				vertices.push_back(-width / 2.0f + j);        // v.z
			}
		}


		stbi_image_free(data);

		std::vector<unsigned int> indices;
		for (unsigned int i = 0; i < height - 1; i++)       // for each row a.k.a. each strip
		{
			for (unsigned int j = 0; j < width; j++)      // for each column
			{
				for (unsigned int k = 0; k < 2; k++)      // for each side of the strip
				{
					indices.push_back(j + width * (i + k));
				}
			}
		}

		const unsigned int NUM_STRIPS = height - 1;
		const unsigned int NUM_VERTS_PER_STRIP = width * 2;

		// register

		GLuint terrainVAO, terrainVBO, terrainEBO;
		glGenVertexArrays(1, &terrainVAO);
		glBindVertexArray(terrainVAO);

		glGenBuffers(1, &terrainVBO);
		glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
		glBufferData(GL_ARRAY_BUFFER,
			vertices.size() * sizeof(float),       // size of vertices buffer
			&vertices[0],                          // pointer to first element
			GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &terrainEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			indices.size() * sizeof(unsigned int), // size of indices buffer
			&indices[0],                           // pointer to first element
			GL_STATIC_DRAW);

		//uniform

		glm::mat4 view = glm::lookAt(
			glm::vec3(0.0f, 20.0f, 50.0f), // eye
			glm::vec3(0.0f, 0.0f, 0.0f),   // center
			glm::vec3(0.0f, 1.0f, 0.0f)    // up
		);
		glm::mat4 projection = glm::perspective(
			glm::radians(90.0f),
			(float)m_window.getWindowWidth() / (float)m_window.getWindowHeigth(),
			0.1f,
			1000.0f
		);




		while (!glfwWindowShouldClose(m_window.m_window)) {
			glfwPollEvents();

			glClearColor(0.f, 0.f, 0.f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT);

			//set uniform
			shaderprogram.setUniform("view", view);
			shaderprogram.setUniform("projection", projection);

			// UPDATE

			m_imgui.Update();

			// DRAW

			shaderprogram.Activate();

			glBindVertexArray(terrainVAO);
			// render the mesh triangle strip by triangle strip - each row at a time
			for (unsigned int strip = 0; strip < NUM_STRIPS; ++strip)
			{
				glDrawElements(GL_LINES,   // primitive type
					NUM_VERTS_PER_STRIP, // number of indices to render
					GL_UNSIGNED_INT,     // index data type
					(void*)(sizeof(unsigned int)
						* NUM_VERTS_PER_STRIP
						* strip)); // offset to starting index
			}

			m_imgui.Render();

			glfwSwapBuffers(m_window.m_window);
		}



		return 0;
	}
}