#pragma once

#include "Voxel.h"

class VoxelWorld
{
public:
	
	VoxelWorld(glm::vec3 origin, int width, int height, int depth, VoxelManager& manager, float m_chunkScale = 1.f, BlockType = EMPTY ,int chunkSize = 16);

	

	MarchingCube GenerateVertices();


	bool isSolid(int x, int y, int z) {
		if (x < 0 || x >= m_Width || y < 0 || y >= m_Height || z < 0 || z >= m_Depth) {
			return false; // Consider out-of-bounds as empty
		}
		return at(x, y, z) != EMPTY;
	}

	BlockType& at(int x, int y, int z);
	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	int GetDepth() const { return m_Depth; }
	float GetScale() const { return m_chunkScale; }
	glm::vec3 GetPosition() const { return m_position; }


private:
	std::unordered_map<int, std::shared_ptr<Chunks>> m_chunks;
	VoxelManager& m_manager;
	glm::vec3 m_position;
	glm::vec3 m_worldOrigin;
	glm::vec3 m_firstChunkPos;

	int m_Width;
	int m_Height;
	int m_Depth;
	int m_chunkSize;
	float m_chunkScale;
};

