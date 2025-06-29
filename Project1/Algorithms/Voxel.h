#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include "Mesh.h"
#include "Shader.h"

enum BlockType {
	EMPTY = 0,
	SOLID = 1
};

enum Face {
	POS_X = 0, // +X
	NEG_X = 1, // -X
	POS_Y = 2, // +Y
	NEG_Y = 3, // -Y
	POS_Z = 4, // +Z
	NEG_Z = 5  // -Z
};

struct Quad {
	int x, y, z;
	int width, height;
	Face face;
	BlockType type;
};

class Chunks {
public:
	Chunks(int _x, int _y, int _z, int chunkWidth, int chunkHeigth, int chunkDepth, float chunkScale = 1.f):
		m_X(_x - chunkWidth / 2), m_Y(_y - chunkHeigth / 2), m_Z(_z - chunkDepth / 2), 
		m_Width(chunkWidth), m_Height(chunkHeigth), m_Depth(chunkDepth), m_ScaleFactor(chunkScale)
	{
		m_Blocks.resize(m_Width * m_Height * m_Depth, SOLID); // Initialize all blocks to SOLID
	}


	std::vector<Quad> GenerateQuads();


	BlockType& at(int x, int y, int z) {
		return m_Blocks[x + y * m_Width + z * m_Width * m_Height];
	}

	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	int GetDepth() const { return m_Depth; }
	float GetScale() const { return m_ScaleFactor; }
   

private:

	// Helper function to check if a block is solid
	bool isSolid(int x, int y, int z) {
		if (x < 0 || x >= m_Width || y < 0 || y >= m_Height || z < 0 || z >= m_Depth) {
			return false; // Consider out-of-bounds as empty
		}
		return at(x, y, z) != EMPTY;
	}


	// Check if a face should be rendered (adjacent block is empty or out of bounds)
	bool shouldRenderFace(int x, int y, int z, int face) {
		int dx[] = { 1, -1, 0, 0, 0, 0 };  // +X, -X, +Y, -Y, +Z, -Z
		int dy[] = { 0, 0, 1, -1, 0, 0 };
		int dz[] = { 0, 0, 0, 0, 1, -1 };

		int nx = x + dx[face];
		int ny = y + dy[face];
		int nz = z + dz[face];

		return isSolid(x, y, z) && !isSolid(nx, ny, nz);
	}

private:
	int m_X, m_Y, m_Z; // Position of the chunk in the world
	int m_Width, m_Height, m_Depth; // Dimensions of the chunk
	float m_ScaleFactor; // Scale factor for rendering

	std::vector<BlockType> m_Blocks; // 3D vector to hold block types

	
};

class Voxel {
public:
	Voxel(Shader& shader);// load graphics
	~Voxel() = default;


	void UpdateChunk(int key);
	void DrawVoxel(const glm::mat4& vp, const glm::vec4& color, GLenum mode);


	int AddChunk(int x, int y, int z, int width, int height, int depth, float scale = 1.f) {
		int key = x + (y << 8) + (z << 24);
		m_Chunks[key] = std::make_unique<Chunks>(x, y, z, width, height, depth, scale);

		return key;
	}

	std::shared_ptr<Chunks> GetChunk(int x, int y, int z) {
		int key = x + (y << 8) + (z << 24);
		auto it = m_Chunks.find(key);
		if (it != m_Chunks.end()) {
			return it->second;
		}
		return nullptr;
	}

	std::shared_ptr<Chunks> GetChunk(int key) {
		auto it = m_Chunks.find(key);
		if (it != m_Chunks.end()) {
			return it->second;
		}
		return nullptr;
	}

private:



private:
	std::unordered_map<int, std::shared_ptr<Chunks>> m_Chunks; // Map to hold chunks by their position
	std::unordered_map<int, std::vector<glm::mat4x4>> m_ChunkTransforms; // Map to hold transforms for each chunk
	std::vector<glm::mat4x4> m_transforms;
	std::unique_ptr<Mesh> m_InstancedQuad;

	bool m_modified;
	Shader& m_Shader; // Reference to the shader for rendering
};
