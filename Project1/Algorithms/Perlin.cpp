#include "Perlin.hpp"
#include "imgui.h"

void PerlinNoise::Update() {
	if (!m_chunk) { return; }
	for (int z = 0; z < m_depth; ++z) {
		for (int y = 0; y < m_height; ++y) {
			for (int x = 0; x < m_width; ++x) {
				float nx = static_cast<float>(x) / m_width  * m_params.scale;
				float ny = static_cast<float>(y) / m_height * m_params.scale;
				float nz = static_cast<float>(z) / m_depth  * m_params.scale;

				// Note:: glm::perlin returns [-1, 1] -> compute perlin noise value in [0, 1] 
				float d = glm::perlin(glm::vec3{ nx, ny, nz }) * 0.5f + 0.5f;

				// Carve if below threshold (geddit we carve Yh x Jaz on the tree)
				m_chunk->at(x, y, z) = (d < m_params.threshold) ? EMPTY : SOLID;
			}
		}
	}
}

void PerlinNoise::DrawImgui() {
	if (ImGui::TreeNode("Perlin Noise Parameters")) {
		ImGui::SliderFloat("Scale", &m_params.scale, 0.001f, 0.2f, "%.3f");
		ImGui::SliderFloat("Threshold", &m_params.threshold, 0.0f, 1.0f);
		ImGui::TreePop();
	}
}

void PerlinNoise::SetChunk(std::shared_ptr<Chunks> chunk, const glm::vec3& worldMin, const glm::vec3& worldSize) {
	m_chunk		= chunk;
	m_worldMin	= worldMin;
	m_worldSize = worldSize;
	m_width		= chunk->GetWidth();
	m_height	= chunk->GetHeight();
	m_depth		= chunk->GetDepth();
}

void PerlinWorm::Update() {
	if (!m_chunk) { return; }
	ApplyPerlin();
	CarveWorms();
}

void PerlinWorm::DrawImgui() {
	if (ImGui::TreeNode("Perlin Worm Parameters")) {
		ImGui::SliderFloat("Scale", &m_params.scale, 0.01f, 0.2f);
		ImGui::SliderFloat("Threshold", &m_params.threshold, 0.0f, 1.0f);
		ImGui::SliderInt("Worm Count", &m_params.wormCount, 0, 20);
		ImGui::SliderInt("Worm Radius", &m_params.wormRadius, 1, 5);
		ImGui::TreePop();
	}
}

void PerlinWorm::SetChunk(std::shared_ptr<Chunks> chunk, const glm::vec3& worldMin, const glm::vec3& worldSize) {
	m_chunk = chunk;
	m_worldMin = worldMin;
	m_worldSize = worldSize;
	m_width = chunk->GetWidth();
	m_height = chunk->GetHeight();
	m_depth = chunk->GetDepth();
}

void PerlinWorm::ApplyPerlin() {
	for (int z = 0; z < m_depth; ++z) {
		for (int y = 0; y < m_height; ++y) {
			for (int x = 0; x < m_width; ++x) {
				float nx = static_cast<float>(x) / m_width * m_params.scale;
				float ny = static_cast<float>(y) / m_height * m_params.scale;
				float nz = static_cast<float>(z) / m_depth * m_params.scale;
				float d = glm::perlin(glm::vec3{ nx, ny, nz }) * 0.5f + 0.5f;
				m_chunk->at(x, y, z) = (d < m_params.threshold) ? EMPTY : SOLID;
			}
		}
	}
}

void PerlinWorm::CarveWorms() {
	if (m_rooms.size() < 2) { return; }

	std::mt19937 rng(std::random_device{}());
	std::uniform_int_distribution<int> random{ 0, int(m_rooms.size() - 1) };

	for (int i = 0; i < m_params.wormCount; ++i) {
		const auto& a = m_rooms[random(rng)];
		const auto& b = m_rooms[random(rng)];

		glm::vec3 start = a.center + a.extent * 0.5f;
		glm::vec3 end = b.center + b.extent * 0.5f;

		glm::vec3 dir = glm::normalize(end - start);
		float len = glm::length(end - start);

		// walk from start to end, carving a sphere at each step
		for (float t = 0; t < len; t += 1.0f) {
			glm::vec3 p = start + dir * t;

			// convert world-space to voxel-space
			glm::vec3 norm = (p - m_worldMin) / m_worldSize;
			glm::ivec3 cell = {
				glm::clamp(int(norm.x * m_width),  0, m_width - 1),
				glm::clamp(int(norm.y * m_height), 0, m_height - 1),
				glm::clamp(int(norm.z * m_depth),  0, m_depth - 1)
			};

			// carve out a sphere of empty space at this cell cause why tf not
			CarveSphere(cell, m_params.wormRadius);
		}
	}
}

// P.S. I saw this and thought it was cool so ill use it hehe
// Carves a solid sphere of EMPTY blocks at the center
void PerlinWorm::CarveSphere(const glm::ivec3& center, int radius) {
	for (int z = -radius; z <= radius; ++z) {
		for (int y = -radius; y <= radius; ++y) {
			for (int x = -radius; x <= radius; ++x) {
				glm::ivec3 p = center + glm::ivec3{ x, y, z };

				// check if outofbounds if yes skip it baby
				if (p.x < 0 || p.y < 0 || p.z < 0 ||
					p.x >= m_width || p.y >= m_height || p.z >= m_depth) {
					continue;
				}

				if (glm::length(glm::vec3{ x, y, z }) <= radius) {
					m_chunk->at(p.x, p.y, p.z) == EMPTY;
				}
			}
		}
	}
}