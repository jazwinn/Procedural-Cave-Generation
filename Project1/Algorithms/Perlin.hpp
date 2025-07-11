#pragma once

#ifndef PERLIN_HPP
#define PERLIN_HPP

#include <random>

#include "BinarySpacePartition.h"
#include "Generator.h"
#include "RandomAlgo.h"

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

inline WormParams m_params;

//Perform Ken Perlin Algorithm
float Perlin(float x, float y, float z);

float PerlinOctave(float x, float y, float z, float octave, float persistence);

template <typename voxelType>
void GeneratePerlin(voxelType& voxelWorld, const glm::ivec3& start, const glm::ivec3& size, unsigned int seed = 1234567, BlockType type = EMPTY) {
	srand(seed);

	float seedX = RandomClass::randomFloat(0.f, 100.f);
	float seedY = RandomClass::randomFloat(0.f, 100.f);
	float seedZ = RandomClass::randomFloat(0.f, 100.f);

	glm::vec3 denominator = glm::vec3(m_params.scale) / glm::vec3(size);

	for (int z = 0; z < size.z; ++z) {
		for (int y = 0; y < size.y; ++y) {
			for (int x = 0; x < size.x; ++x) {

				int coordX = start.x + x;
				int coordY = start.y + y;
				int coordZ = start.z + z;

				float nx = static_cast<float>(x) * denominator.x + seedX;
				float ny = static_cast<float>(y) * denominator.y + seedY;
				float nz = static_cast<float>(z) * denominator.z + seedZ;
				float d = PerlinOctave(nx, ny, nz, m_params.octave, m_params.octavePersistence) * 0.5f + 0.5f;
				if (d < m_params.threshold) {
					voxelWorld.at(coordX, coordY, coordZ) = type;
				}

			}
		}
	}
}

void PerlinDrawImgui();

//class PerlinNoise : public Generator {
//public:
//    void Update() override;
//    void Draw(Shapes&, const glm::mat4&, const glm::vec4&) override {}
//    void DrawImgui() override;
//
//    // Note to self: I should really override this and name it initialize or something 
//    // cause that's what I'm literally doing #futureyhproblem
//    void SetChunk(std::shared_ptr<Chunks> chunk, const glm::vec3& worldMin, const glm::vec3& worldSize);
//    NoiseParams& GetParams() { return m_params; }
//private:
//    std::shared_ptr<Chunks> m_chunk;
//    glm::vec3 m_worldMin, m_worldSize;
//    int m_width, m_height, m_depth;
//    NoiseParams m_params;
//};
//
//class PerlinWorm : public Generator {
//public:
//	PerlinWorm(unsigned int& seed) : m_seed(seed) {}
//
//    void Update() override;
//    void Draw(Shapes&, const glm::mat4&, const glm::vec4&) override {}
//    void DrawImgui() override;
//    static void StaticDrawImgui();
//    void SetChunk(std::shared_ptr<Chunks> chunk, const glm::vec3& worldMin, const glm::vec3& worldSize);
//    void SetRooms(const std::vector<BinarySpacePartition::Room>& rooms) { m_rooms = rooms; }
//    void ApplyPerlin(BlockType type = EMPTY);
//    //static WormParams& GetParams() { return m_params; }
//private:
//    std::shared_ptr<Chunks> m_chunk;
//    std::vector<BinarySpacePartition::Room> m_rooms; 
//    glm::vec3 m_worldMin, m_worldSize;
//    int m_width, m_height, m_depth;
//	unsigned int& m_seed;
//
//    
//    void CarveWorms();
//    void CarveSphere(const glm::ivec3& center, int radius);
//};


#endif 