#pragma once

#include <memory>
#include "Mesh.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>

enum DrawType {
	FILLED,
	WIREFRAME,
	FILLEDWIREFRAME
};


class Shapes
{
public:
	Shapes(Shader& shader, Shader& instancedShader);

	void Draw_Rectangle(const glm::mat4& vp, const glm::vec3 center, const glm::vec3 scale, const glm::vec4& color, DrawType drawtype = FILLED, const glm::vec4& wireFrameColor = {0.f,0.f,0.f,1.f});
	void Draw_Quad(const glm::mat4& vp, const glm::vec3 center, const glm::vec3 scale, const glm::vec4& color, DrawType drawtype = FILLED, const glm::vec4& wireFrameColor = { 0.f,0.f,0.f,1.f });
	void Draw_Line(const glm::mat4& vp, const glm::vec3 start, const glm::vec3 end, const glm::vec4& color);


	void Draw_InstancedCubes(const glm::mat4& vp, const glm::vec4& color, DrawType drawtype = FILLED, const glm::vec4& wireFrameColor = { 0.f,0.f,0.f,1.f });
	void Draw_InstancedQuads(const glm::mat4& vp, const glm::vec4& color, DrawType drawtype = FILLED, const glm::vec4& wireFrameColor = { 0.f,0.f,0.f,1.f });


private:

	Shader& m_Shader;
	Shader& m_InstancedShader;

	std::unique_ptr<Mesh> m_RectangleMesh;
	std::unique_ptr<Mesh> m_QuadMesh;
	std::unique_ptr<Mesh> m_LineMesh;

	std::unique_ptr<Mesh> m_InstancedCube;
	std::unique_ptr<Mesh> m_InstancedQuad;
};

