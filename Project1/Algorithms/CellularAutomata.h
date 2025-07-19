#pragma once

#ifndef CELLULARAUTOMATA_H
#define CELLULARAUTOMATA_H

#include <array>
#include <random>
#include <vector>
#include "Generator.h"
#include "BinarySpacePartition.h"
#include "imgui.h"

struct CaParams {
	float initialFillProb = 0.26f;
	int birthMin = 12;
	int birthMax = 26;
	int survivalMin = 10;
	int survivalMax = 26;
	int maxState = 5;
	int iterationCount = 15;
	bool inverted = false;
	bool sphereMode = false;
};

inline CaParams m_caParams;

static const std::array<glm::ivec3, 26> kOffset = [] {
	std::array<glm::ivec3, 26> arr;
	int idx = 0;
	for (int z = -1; z <= 1; ++z)
		for (int y = -1; y <= 1; ++y)
			for (int x = -1; x <= 1; ++x)
				if (x || y || z)
					arr[idx++] = { x, y, z };
	return arr;
	}();

inline BlockType CA_ToBlock(int state, bool inverted) {
	bool alive = state > 0;
	if (inverted) alive = !alive;
	return alive ? SOLID : EMPTY;
}

template<typename VoxelWorldT>
class CellularAutomata {
public:
	CellularAutomata(VoxelWorldT& world, const glm::ivec3& origin, const glm::ivec3& size, unsigned int seed, BlockType fillType) : m_world(world), m_origin(origin), m_size(size), m_rng(seed), m_step(0) {
		int N = size.x * size.y * size.z;
		m_cur.resize(N);
		m_nxt.resize(N);

		std::uniform_real_distribution<float> d01(0.0f, 1.0f);
		std::fill(m_cur.begin(), m_cur.end(), 0);

		glm::ivec3 center = size / 2;
		int clusterRadius = std::min({ size.x, size.y, size.z }) / (m_caParams.sphereMode ? 2 : 1);

		for (int dz = -clusterRadius; dz <= clusterRadius; ++dz)
			for (int dy = -clusterRadius; dy <= clusterRadius; ++dy)
				for (int dx = -clusterRadius; dx <= clusterRadius; ++dx) {
					int cx = center.x + dx;
					int cy = center.y + dy;
					int cz = center.z + dz;
					if (cx < 0 || cx >= size.x || cy < 0 || cy >= size.y || cz < 0 || cz >= size.z) continue;
					if (d01(m_rng) < m_caParams.initialFillProb)
						m_cur[cx + cy * size.x + cz * size.x * size.y] = m_caParams.maxState;
				}
	}

	bool GenerateCellularAutomata() {
		if (m_step >= m_caParams.iterationCount) return false;
		int W = m_size.x, H = m_size.y, D = m_size.z;
		auto idx3 = [&](int x, int y, int z) { return x + y * W + z * W * H; };

		for (int z = 0; z < D; ++z)
			for (int y = 0; y < H; ++y)
				for (int x = 0; x < W; ++x) {
					int i = idx3(x, y, z);
					int state = m_cur[i];
					int liveNeighbors = 0;
					for (auto& o : kOffset) {
						int nx = x + o.x, ny = y + o.y, nz = z + o.z;
						if (nx >= 0 && nx < W && ny >= 0 && ny < H && nz >= 0 && nz < D)
							if (m_cur[idx3(nx, ny, nz)] > 0)
								liveNeighbors++;
					}

					if (state == 0) {
						// Birth rule
						bool born = (liveNeighbors >= m_caParams.birthMin && liveNeighbors <= m_caParams.birthMax);
						m_nxt[i] = born ? m_caParams.maxState : 0;
					}
					else {
						// Survival: keep current state if neighbors in range
						if (liveNeighbors >= m_caParams.survivalMin && liveNeighbors <= m_caParams.survivalMax)
							m_nxt[i] = state;
						else
							// Linear decay: decrement state by 1 per iteration
							m_nxt[i] = state > 0 ? state - 1 : 0;
					}
				}

		std::swap(m_cur, m_nxt);

		for (int z = 0; z < D; ++z)
			for (int y = 0; y < H; ++y)
				for (int x = 0; x < W; ++x) {
					int v = m_cur[idx3(x, y, z)];
					m_world.at(m_origin.x + x, m_origin.y + y, m_origin.z + z) = CA_ToBlock(v, m_caParams.inverted);
				}

		++m_step;
		return true;
	}

private:
	VoxelWorldT& m_world;
	glm::ivec3 m_origin, m_size;
	std::mt19937 m_rng;
	std::vector<int> m_cur, m_nxt;
	int m_step;
};

inline void CellularAutomataDrawImgui() {
	if (ImGui::TreeNode("Ca Params")) {
		ImGui::SliderInt("Birth Min", &m_caParams.birthMin, 0, 26);
		ImGui::SliderInt("Birth Max", &m_caParams.birthMax, 0, 26);
		ImGui::SliderInt("Survive Min", &m_caParams.survivalMin, 0, 26);
		ImGui::SliderInt("Survive Max", &m_caParams.survivalMax, 0, 26);
		ImGui::SliderInt("Max State", &m_caParams.maxState, 1, 10);
		ImGui::SliderInt("Iterations", &m_caParams.iterationCount, 1, 50);
		ImGui::SliderFloat("Initial Fill", &m_caParams.initialFillProb, 0.0f, 1.0f);
		ImGui::Checkbox("Inverted", &m_caParams.inverted);
		ImGui::Checkbox("Sphere Mode", &m_caParams.sphereMode);
		ImGui::TreePop();
	}
}

#endif // CELLULARAUTOMATA_H