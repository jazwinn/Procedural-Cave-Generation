#pragma once

#ifndef CELLULARAUTOMATA_H
#define CELLULARAUTOMATA_H

#include <array>
#include <random>
#include "Generator.h"
#include "BinarySpacePartition.h"
#include "imgui.h"

struct CaParams {
    float initialFillProb = 0.35f;  // 35% chance to start “alive”
    int birthMin = 12;
    int birthMax = 26;
    int survivalMin = 10;
    int survivalMax = 26;
    int maxState = 5;
    int iterationCount = 10;
    bool generateSolid = true;
    bool sphereMode = false;
};

inline CaParams m_caParams;

//static constexpr std::array<bool, 27> kBirth = [] {
//    std::array<bool, 27> a{};
//    a[1] = a[4] = a[8] = a[11] = true;
//    for (int i = 13; i <= 26; ++i) a[i] = true;
//    return a;
//}();

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

template<typename VoxelWorldT>
class CellularAutomata {
public:
    CellularAutomata(VoxelWorldT& world,
        const glm::ivec3& origin,
        const glm::ivec3& size,
        unsigned int seed,
        BlockType fillType)
		: m_world(world), m_origin(origin), m_size(size), m_rng(seed), m_step(0), m_fillType(fillType)
    {
        int N = size.x * size.y * size.z;
        m_cur.resize(N);
        m_nxt.resize(N);

        //// optionally clear the region (based on fillType)
        //for (int z = 0; z < size.z; ++z)
        //    for (int y = 0; y < size.y; ++y)
        //        for (int x = 0; x < size.x; ++x)
        //            world.at(origin.x + x, origin.y + y, origin.z + z) = fillType;

        // seed initial state with 0 or maxState
        std::uniform_real_distribution<float> d01(0.0f, 1.0f);

        // Create a central cluster to start with
        glm::ivec3 center = size / 2;
        int clusterRadius = std::min({ size.x, size.y, size.z }) / (m_caParams.sphereMode ? 2 : 1) ;
        int sqrClusterRadius = clusterRadius * clusterRadius;

        for (int i = 0; i < N; ++i) {
            m_cur[i] = 0; // Start with everything dead
        }

        for (int z = -clusterRadius; z <= clusterRadius; ++z) {
            for (int y = -clusterRadius; y <= clusterRadius; ++y) {
                for (int x = -clusterRadius; x <= clusterRadius; ++x) {
                    if (center.x + x >= 0 && center.x + x < size.x &&
                        center.y + y >= 0 && center.y + y < size.y &&
                        center.z + z >= 0 && center.z + z < size.z) {

                        float sqrDistance = x * x + y * y + z * z;
                        if (sqrDistance <= sqrClusterRadius && d01(m_rng) < m_caParams.initialFillProb) {
                            int idx = (center.x + x) + (center.y + y) * size.x + (center.z + z) * size.x * size.y;
                            m_cur[idx] = m_caParams.maxState;  // Start with max state, not 1
                        }
                    }
                }
            }
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

                    // Count living neighbors (only state 1 cells count as alive)
                    int liveNeighbors = 0;
                    for (const auto& o : kOffset) {
                        int nx = x + o.x;
                        int ny = y + o.y;
                        int nz = z + o.z;

                        // Check bounds - treat out-of-bounds as dead (absorbing boundary)
                        if (nx >= 0 && nx < W && ny >= 0 && ny < H && nz >= 0 && nz < D) {
                            if (m_cur[idx3(nx, ny, nz)] == m_caParams.maxState)
                                liveNeighbors++;
                        }
                    }

                    if (state == 0) {
                        // Birth rule: 1,4,8,11,13-26 neighbors
                        bool born = (liveNeighbors >= m_caParams.birthMin && liveNeighbors <= m_caParams.birthMax);
                        m_nxt[i] = born ? m_caParams.maxState : 0;
                    }
                    else if (state == m_caParams.maxState) {
                        // Survival rule for fully alive cells: 13-26 neighbors
                        bool survive = (liveNeighbors >= m_caParams.survivalMin &&
                            liveNeighbors <= m_caParams.survivalMax);
                        m_nxt[i] = survive ? m_caParams.maxState : (m_caParams.maxState - 1);  // Stay alive or start dying
                    }
                    else {
                        // Dying states: decay by 1 each step until reaching 0
                        m_nxt[i] = (state - 1 < 0) ? 0 : (state - 1);
                    }
                }

        std::swap(m_cur, m_nxt);

        for (int z = 0; z < D; ++z)
            for (int y = 0; y < H; ++y)
                for (int x = 0; x < W; ++x) {
                    int state = m_cur[idx3(x, y, z)];
                    if (state > 0) {
                        m_world.at(m_origin.x + x, m_origin.y + y, m_origin.z + z) = m_fillType;
                    }
                    
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
	BlockType m_fillType;
};

inline void CellularAutomataDrawImgui() {
	ImGui::SliderFloat("Fill Probability", &m_caParams.initialFillProb, 0.0f, 1.0f);
	ImGui::SliderInt("Birth Min", &m_caParams.birthMin, 0, 26);
	ImGui::SliderInt("Birth Max", &m_caParams.birthMax, 0, 26);
	ImGui::SliderInt("Survive Min", &m_caParams.survivalMin, 0, 26);
	ImGui::SliderInt("Survive Max", &m_caParams.survivalMax, 0, 26);
	ImGui::SliderInt("Max State", &m_caParams.maxState, 1, 10);
	ImGui::SliderInt("Iterations", &m_caParams.iterationCount, 1, 50);
	ImGui::Checkbox("Generate Solid", &m_caParams.generateSolid);
	ImGui::Checkbox("Sphere Mode", &m_caParams.sphereMode);
}

#endif 