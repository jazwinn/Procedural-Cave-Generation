#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>

#include "Mesh.h"
#include "Shader.h"

class Voxel;


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

struct VoxelConfig {
	bool greedy = true;
};

struct Quad {
	int x, y, z;
	int width, height;
	Face face;
	BlockType type;
};

class Chunks {
public:
	Chunks(float _x, float _y, float _z, int chunkWidth, int chunkHeight, int chunkDepth, float chunkScale = 1.f, BlockType type = EMPTY) :
		m_X(_x - (chunkWidth * chunkScale) / 2.f + chunkScale * 0.5f),
		m_Y(_y - (chunkHeight * chunkScale) / 2.f + chunkScale * 0.5f),
		m_Z(_z - (chunkDepth * chunkScale) / 2.f + chunkScale * 0.5f),
		m_Width(chunkWidth), m_Height(chunkHeight), m_Depth(chunkDepth), m_ScaleFactor(chunkScale)
	{
		m_Blocks.resize(m_Width * m_Height * m_Depth, type); // Initialize all blocks to EMPTY
	}


	std::vector<Quad> GenerateQuads();
	std::vector<Quad> GenerateQuadsGreedy();

	BlockType& at(int x, int y, int z) {
		return m_Blocks[x + y * m_Width + z * m_Width * m_Height];
	}

	void SetWorld(Voxel* voxel) {
		m_Voxel = voxel;
	}

	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	int GetDepth() const { return m_Depth; }
	float GetScale() const { return m_ScaleFactor; }
	glm::vec3 GetPosition() const { return glm::vec3(m_X, m_Y, m_Z); }


	std::optional<std::reference_wrapper<BlockType>>  GetWorldBlock(int x, int y, int z);
	void FillChunk(BlockType type);

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
	float m_X, m_Y, m_Z; // Position of the chunk in the world
	int m_Width, m_Height, m_Depth; // Dimensions of the chunk
	float m_ScaleFactor; // Scale factor for rendering

	std::vector<BlockType> m_Blocks; // 3D vector to hold block types

	Voxel* m_Voxel = nullptr;


};

class Voxel {
public:
	Voxel(Shader& shader);// load graphics
	~Voxel() = default;


	void UpdateChunk(int key);
	void UpdateAllChunk();
	void DrawVoxel(const glm::mat4& vp, const glm::vec4& color, GLenum mode);


	void deleteChunk(int key);
	void clearVoxel();

	int AddChunk(float x, float y, float z, int width, int height, int depth, float scale = 1.f, BlockType type = EMPTY) {
		int key = (int)x + ((int)y << 8) + ((int)z << 24);
		m_Chunks[key] = std::make_unique<Chunks>(x, y, z, width, height, depth, scale, type);

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

public:
	VoxelConfig config;
	glm::vec4 color{ 0.7,0.7,0.7,0.2 };

private:
	std::unordered_map<int, std::shared_ptr<Chunks>> m_Chunks; // Map to hold chunks by their position
	std::unordered_map<int, std::vector<glm::mat4x4>> m_ChunkTransforms; // Map to hold transforms for each chunk
	std::vector<glm::mat4x4> m_transforms;
	std::unique_ptr<Mesh> m_InstancedQuad;

	bool m_modified;
	Shader& m_Shader; // Reference to the shader for rendering
};
