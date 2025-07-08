#include "VoxelWorld.h"

VoxelWorld::VoxelWorld(glm::vec3 origin, int width, int height, int depth, VoxelManager& manager , float chunkScale, BlockType type,  int chunkSize):
	m_manager(manager),
	m_worldOrigin(origin),
	m_chunkSize(chunkSize),
	m_chunkScale(chunkScale)
{

	if (width% chunkSize || depth% chunkSize || height% chunkSize) {
		assert("size not mupltiple of %d", cubeSize);
	}

	glm::vec3 worldBottomLeft = m_worldOrigin - glm::vec3{ (float)width, (float)height, (float)depth } * chunkScale * 0.5f;
	m_firstChunkPos = worldBottomLeft + glm::vec3{ chunkSize, chunkSize, chunkSize } *0.5f * chunkScale;

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

BlockType& VoxelWorld::at(int wx, int wy, int wz)
{
	

	// Step 1: Determine chunk coordinates
	int chunkX = wx / m_chunkSize;
	int chunkY = wy / m_chunkSize;
	int chunkZ = wz / m_chunkSize;

	////get chunk center
	float interval = m_chunkScale * m_chunkSize;
	glm::vec3 chunkPos = m_firstChunkPos + glm::vec3{ interval * chunkX, interval * chunkY, interval * chunkZ };
	

	//// Handle negative positions correctly
	if (wx < 0) chunkX -= 1;
	if (wy < 0) chunkY -= 1;
	if (wz < 0) chunkZ -= 1;

	//// Step 2: Convert to chunk index
	//int localChunkX = chunkX - origin.x;
	//int localChunkY = chunkY - origin.y;
	//int localChunkZ = chunkZ - origin.z;

	//// Bounds check
	//if (localChunkX < 0 || localChunkX >= width ||
	//	localChunkY < 0 || localChunkY >= height ||
	//	localChunkZ < 0 || localChunkZ >= depth) {
	//	throw std::out_of_range("Block is outside the world bounds.");
	//}

	auto chunk = m_manager.GetChunk(chunkPos.x, chunkPos.y, chunkPos.z);

	//// Step 3: Compute local block position inside the chunk
	//int lx = wx % CHUNK_SIZE;
	//int ly = wy % CHUNK_SIZE;
	//int lz = wz % CHUNK_SIZE;

	//if (lx < 0) lx += CHUNK_SIZE;
	//if (ly < 0) ly += CHUNK_SIZE;
	//if (lz < 0) lz += CHUNK_SIZE;

	//// Step 4: Access the voxel inside the chunk
	//return chunk.GetVoxel(lx, ly, lz); // Reference so you can read/write

	return chunk->at(wx % m_chunkSize, wy % m_chunkSize, wz % m_chunkSize); // Reference so you can read/write
}
