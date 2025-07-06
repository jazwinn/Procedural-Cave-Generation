#pragma once

#ifndef CELLULARAUTOMATA_H
#define CELLULARAUTOMATA_H

#include <random>
#include "Generator.h"
#include "BinarySpacePartition.h"

struct CAParams {
	float wallProb = 0.45f;		// initial wall probability [0..1]
	int   birthLimit = 4;		// walls born if neighbors > this
	int   deathLimit = 3;		// walls die if neighbors < this
	int   maxIterations = 10;
};

class CellularAutomata : public Generator {
public:
    CellularAutomata(unsigned int& seed) : m_seed{ seed } {};
    void SetChunk(std::shared_ptr<Chunks> chunk, const glm::vec3& worldMin, const glm::vec3& worldSize);
    void SetSeeds(const std::vector<BinarySpacePartition::Room>& rooms);
    void Update() override;
    void Draw(Shapes& shapes, const glm::mat4& vp, const glm::vec4& color) override;
    void DrawImgui() override;
    CAParams& GetParams() { return m_params; }
    bool RunIteration();
    bool IsComplete() const { return m_currentIter >= m_params.maxIterations || m_completed; }
private:
    std::shared_ptr<Chunks> m_chunk;
    glm::vec3 m_worldMin;
    glm::vec3 m_worldSize;
    int m_width, m_height, m_depth;
    std::vector<int> m_grid;      
    std::vector<int> m_buffer;    
    CAParams m_params;
    int m_currentIter{ 0 };
    bool m_completed{ false };
	unsigned int& m_seed;

};

#endif 