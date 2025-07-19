#include "VoxelWorld.h"
#include <array>

VoxelWorld::VoxelWorld(glm::vec3 origin, int width, int height, int depth, VoxelManager& manager , float chunkScale, BlockType type,  int chunkSize):
	m_manager(manager),
	m_worldOrigin(origin),
	m_chunkSize(chunkSize),
	m_chunkScale(chunkScale),
	m_Width(width),
	m_Height(height),
	m_Depth(depth),
	m_position(m_worldOrigin - glm::vec3{ (float)width, (float)height, (float)depth } *chunkScale * 0.5f) {

	if (width% chunkSize || depth% chunkSize || height% chunkSize) {
		assert("size not mupltiple of %d", cubeSize);
	}

	glm::vec3 worldBottomLeft = m_worldOrigin - glm::vec3{ (float)width, (float)height, (float)depth } * chunkScale * 0.5f;
	m_firstChunkPos = m_position + glm::vec3{ chunkSize, chunkSize, chunkSize } *0.5f * chunkScale;

	float interval = chunkScale * chunkSize;
	int iterationX = width / chunkSize;
	int iterationY = height / chunkSize;
	int iterationZ = depth / chunkSize;

	for (int x = 0; x < iterationX; ++x) {
		for (int y = 0; y < iterationY; ++y) {
			for (int z = 0; z < iterationZ; ++z) {
				glm::vec3 chunkPos = m_firstChunkPos + glm::vec3{ interval * x, interval * y, interval * z};

				int key = manager.AddChunk(chunkPos.x, chunkPos.y, chunkPos.z, chunkSize, chunkSize, chunkSize, chunkScale, type);
				m_chunks[key] = manager.GetChunk(key);
			}
		}
	}

	manager.UpdateAllChunk();
}

BlockType& VoxelWorld::at(int wx, int wy, int wz) {
	int chunkX = wx / m_chunkSize;
	int chunkY = wy / m_chunkSize;
	int chunkZ = wz / m_chunkSize;

	// Get chunk center
	float interval = m_chunkScale * m_chunkSize;
	glm::vec3 chunkPos = m_firstChunkPos + glm::vec3{ interval * chunkX, interval * chunkY, interval * chunkZ };
	
	if (wx < 0) { chunkX -= 1; }
	if (wy < 0) { chunkY -= 1; }
	if (wz < 0) { chunkZ -= 1; }

	auto chunk = m_manager.GetChunk(chunkPos.x, chunkPos.y, chunkPos.z);

	return chunk->at(wx % m_chunkSize, wy % m_chunkSize, wz % m_chunkSize); // Reference so you can read/write
}

MarchingCube VoxelWorld::GenerateVertices() {
	MarchingCube marchingCube;
	marchingCube.position = GetPosition();
	marchingCube.scale = GetScale();

	float isoLevel = 0.5f; // The isolevel for the marching cubes algorithm

	marchingCube.vertices.reserve(m_Width * m_Height * m_Depth * 8);
	marchingCube.vertices.reserve(m_Width * m_Height * m_Depth * 8);
	int countVertex = 0;

	// Get the chunk pos based on the bottom left;
	glm::vec3 chunkSize = glm::vec3{ (float)GetWidth(), (float)GetHeight(), (float)GetDepth() };
	glm::vec3 pos = GetPosition();
	glm::vec3 chunkPos = pos - glm::vec3(0.5 * m_chunkScale) + (chunkSize / 2.f);

	for (int x = 0; x < m_Width; ++x) {
		for (int y = 0; y < m_Height; ++y) {
			for (int z = 0; z < m_Depth; ++z) {
				std::array<glm::vec3, 8> cubeCorners;

				for (int i = 0; i < 8; ++i) {
					int cx = x + static_cast<int>(cornerOffsets[i].x);
					int cy = y + static_cast<int>(cornerOffsets[i].y);
					int cz = z + static_cast<int>(cornerOffsets[i].z);

					glm::vec3 localOffset = glm::vec3(cx, cy, cz);
					cubeCorners[i] = localOffset;
				}

				unsigned int lookUpIndex = 0;

				if (IsSolid(x, y, z))			  lookUpIndex |= 0b00000001;	// Corner 0
				if (IsSolid(x + 1, y, z))		  lookUpIndex |= 0b00000010;	// Corner 1
				if (IsSolid(x + 1, y, z + 1))	  lookUpIndex |= 0b00000100;	// Corner 2
				if (IsSolid(x, y, z + 1))		  lookUpIndex |= 0b00001000;	// Corner 3
				if (IsSolid(x, y + 1, z))		  lookUpIndex |= 0b00010000;	// Corner 4
				if (IsSolid(x + 1, y + 1, z))	  lookUpIndex |= 0b00100000;	// Corner 5
				if (IsSolid(x + 1, y + 1, z + 1)) lookUpIndex |= 0b01000000;	// Corner 6
				if (IsSolid(x, y + 1, z + 1))	  lookUpIndex |= 0b10000000;	// Corner 7

				const int* edge = triTable[lookUpIndex];
				for (int n{}; edge[n] != -1; n += 3) {
					int a0 = cornerIndexAFromEdge[triTable[lookUpIndex][n]];
					int b0 = cornerIndexBFromEdge[triTable[lookUpIndex][n]];

					int a1 = cornerIndexAFromEdge[triTable[lookUpIndex][n + 1]];
					int b1 = cornerIndexBFromEdge[triTable[lookUpIndex][n + 1]];

					int a2 = cornerIndexAFromEdge[triTable[lookUpIndex][n + 2]];
					int b2 = cornerIndexBFromEdge[triTable[lookUpIndex][n + 2]];

					glm::vec3 v0 = glm::mix(cubeCorners[a0], cubeCorners[b0], isoLevel);
					glm::vec3 v1 = glm::mix(cubeCorners[a1], cubeCorners[b1], isoLevel);
					glm::vec3 v2 = glm::mix(cubeCorners[a2], cubeCorners[b2], isoLevel);

					marchingCube.vertices.push_back(v0);
					marchingCube.vertices.push_back(v1);
					marchingCube.vertices.push_back(v2);

					marchingCube.indices.push_back(countVertex);
					marchingCube.indices.push_back(countVertex + 2);
					marchingCube.indices.push_back(countVertex + 1);

					countVertex += 3;
				}
			}
		}
	}

	return marchingCube;
}
