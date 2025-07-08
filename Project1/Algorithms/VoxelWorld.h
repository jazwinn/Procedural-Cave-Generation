#pragma once

#include "Voxel.h"

class VoxelWorld
{
public:
	
	VoxelWorld(glm::vec3 origin, int width, int height, int depth, VoxelManager& manager, float m_chunkScale = 1.f, BlockType = EMPTY ,int chunkSize = 16);

	BlockType& at(int x, int y, int z);



private:
	std::unordered_map<int, std::shared_ptr<Chunks>> m_chunks;
	VoxelManager& m_manager;
	glm::vec3 m_worldOrigin;
	glm::vec3 m_firstChunkPos;

	int m_chunkSize;
	float m_chunkScale;
};

