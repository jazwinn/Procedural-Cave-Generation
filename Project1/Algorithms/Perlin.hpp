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
	bool generateSolid = false;		// Generate solid blocks or empty space
};

inline WormParams perlinParams;

// Perform Ken Perlin Algorithm
float Perlin(float x, float y, float z);

float PerlinOctave(float x, float y, float z, float octave, float persistence);

template <typename voxelType>
void GeneratePerlin(voxelType& voxelWorld, const glm::ivec3& start, const glm::ivec3& size, unsigned int seed = 1234567, BlockType type = EMPTY) {
	srand(seed);

	float seedX = rdm::RandomFloat(0.f, 100.f);
	float seedY = rdm::RandomFloat(0.f, 100.f);
	float seedZ = rdm::RandomFloat(0.f, 100.f);

	glm::vec3 denominator = glm::vec3(perlinParams.scale) / glm::vec3(size);

	for (int z = 0; z < size.z; ++z) {
		for (int y = 0; y < size.y; ++y) {
			for (int x = 0; x < size.x; ++x) {

				int coordX = start.x + x;
				int coordY = start.y + y;
				int coordZ = start.z + z;

				float nx = static_cast<float>(x) * denominator.x + seedX;
				float ny = static_cast<float>(y) * denominator.y + seedY;
				float nz = static_cast<float>(z) * denominator.z + seedZ;
				float d = PerlinOctave(nx, ny, nz, perlinParams.octave, perlinParams.octavePersistence) * 0.5f + 0.5f;
				if (d < perlinParams.threshold) {
					voxelWorld.at(coordX, coordY, coordZ) = type;
				}

			}
		}
	}
}

void PerlinDrawImgui();

#endif 