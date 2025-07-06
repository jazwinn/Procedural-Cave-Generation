#pragma once

#ifndef PERLIN_HPP
#define PERLIN_HPP

#include <random>

#include "BinarySpacePartition.h"
#include "Generator.h"
#include "glm/gtc/noise.hpp"

struct NoiseParams {
	float scale;        // Frequency of noise (Lower = wider features)
	float threshold;    // Carving threshold  (Lower = more empty space)  
    int octave = 1;
    float octavePersistence = 0.3f;
};

struct WormParams {
    float scale     = 4.f;
    float threshold = 0.75f;
    int wormCount   = 5;
    int wormRadius  = 2;
    int octave = 1;
    float octavePersistence = 1.f;
};



//Perform Ken Perlin Algorithm
float Perlin(float x, float y, float z);

float PerlinOctave(float x, float y, float z, float octave, float persistence);


class PerlinNoise : public Generator {
public:
    void Update() override;
    void Draw(Shapes&, const glm::mat4&, const glm::vec4&) override {}
    void DrawImgui() override;

    // Note to self: I should really override this and name it initialize or something 
    // cause that's what I'm literally doing #futureyhproblem
    void SetChunk(std::shared_ptr<Chunks> chunk, const glm::vec3& worldMin, const glm::vec3& worldSize);
    NoiseParams& GetParams() { return m_params; }
private:
    std::shared_ptr<Chunks> m_chunk;
    glm::vec3 m_worldMin, m_worldSize;
    int m_width, m_height, m_depth;
    NoiseParams m_params;
};

class PerlinWorm : public Generator {
public:
    void Update() override;
    void Draw(Shapes&, const glm::mat4&, const glm::vec4&) override {}
    void DrawImgui() override;
    static void StaticDrawImgui();
    void SetChunk(std::shared_ptr<Chunks> chunk, const glm::vec3& worldMin, const glm::vec3& worldSize);
    void SetRooms(const std::vector<BinarySpacePartition::Room>& rooms) { m_rooms = rooms; }
    void ApplyPerlin(BlockType type = EMPTY);
    static WormParams& GetParams() { return m_params; }
private:
    std::shared_ptr<Chunks> m_chunk;
    std::vector<BinarySpacePartition::Room> m_rooms; 
    glm::vec3 m_worldMin, m_worldSize;
    int m_width, m_height, m_depth;
    static WormParams m_params;

    
    void CarveWorms();
    void CarveSphere(const glm::ivec3& center, int radius);
};

#endif 