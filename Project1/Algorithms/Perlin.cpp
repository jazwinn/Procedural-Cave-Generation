#include "Perlin.hpp"
#include "Imgui.h"


static const int permutation[256] = {
	151, 160, 137,  91,  90,  15, 131,  13, 201,  95,  96,  53, 194, 233,   7, 225,
	140,  36, 103,  30,  69, 142,   8,  99,  37, 240,  21,  10,  23, 190,   6, 148,
	247, 120, 234,  75,   0,  26, 197,  62,  94, 252, 219, 203, 117,  35,  11,  32,
	57, 177,  33,  88, 237, 149,  56,  87, 174,  20, 125, 136, 171, 168,  68, 175,
	74, 165,  71, 134, 139,  48,  27, 166,  77, 146, 158, 231,  83, 111, 229, 122,
	60, 211, 133, 230, 220, 105,  92,  41,  55,  46, 245,  40, 244, 102, 143,  54,
	65,  25,  63, 161,   1, 216,  80,  73, 209,  76, 132, 187, 208,  89,  18, 169,
	200, 196, 135, 130, 116, 188, 159,  86, 164, 100, 109, 198, 173, 186,   3,  64,
	52, 217, 226, 250, 124, 123,   5, 202,  38, 147, 118, 126, 255,  82,  85, 212,
	207, 206,  59, 227,  47,  16,  58,  17, 182, 189,  28,  42, 223, 183, 170, 213,
	119, 248, 152,   2,  44, 154, 163,  70, 221, 153, 101, 155, 167,  43, 172,   9,
	129,  22,  39, 253,  19,  98, 108, 110,  79, 113, 224, 232, 178, 185, 112, 104,
	218, 246,  97, 228, 251,  34, 242, 193, 238, 210, 144,  12, 191, 179, 162, 241,
	81,  51, 145, 235, 249,  14, 239, 107,  49, 192, 214,  31, 181, 199, 106, 157,
	184,  84, 204, 176, 115, 121,  50,  45, 127,   4, 150, 254, 138, 236, 205,  93,
	222, 114,  67,  29,  24,  72, 243, 141, 128, 195,  78,  66, 215,  61, 156, 180
};

int Inc(int num) {
	num++;
	if (num >= 512) {
		return num % 512;
	}

	return num;
}

float Fade(double t) {
	return t * t * t * (t * (t * 6 - 15) + 10);      
}

float Lerp(float a, float b, float x) {
	return a + x * (b - a);
}

float Gradient(int hash, float x, float y, float z) {
	int h = hash & 15;
	float u = h < 8 ? x : y;

	float v;

	if (h < 4) {
		v = y;
	}
	else if (h == 12 || h == 14) {
		v = x;
	}
	else {
		v = z;
	}

	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float Perlin(float x, float y, float z)
{
	 static int p[512];
    static bool initialized = false;
    if (!initialized) {
        for (int i = 0; i < 512; i++) {
            p[i] = permutation[i % 256];
        }
        initialized = true;
    }

	int xi = (int)x & 255;                         
	int yi = (int)y & 255;                             
	int zi = (int)z & 255;                            
	double xf = x - (int)x;
	double yf = y - (int)y;
	double zf = z - (int)z;

	double u = Fade(xf);
	double v = Fade(yf);
	double w = Fade(zf);

	int aaa, aba, aab, abb, baa, bba, bab, bbb;
	aaa = p[p[p[xi] + yi] + zi];
	aba = p[p[p[xi] + Inc(yi)] + zi];
	aab = p[p[p[xi] + yi] + Inc(zi)];
	abb = p[p[p[xi] + Inc(yi)] + Inc(zi)];
	baa = p[p[p[Inc(xi)] + yi] + zi];
	bba = p[p[p[Inc(xi)] + Inc(yi)] + zi];
	bab = p[p[p[Inc(xi)] + yi] + Inc(zi)];
	bbb = p[p[p[Inc(xi)] + Inc(yi)] + Inc(zi)];

	double x1, x2, y1, y2;
	x1 = Lerp(Gradient(aaa, xf, yf, zf), Gradient(baa, xf - 1, yf, zf),u);     

	x2 = Lerp(Gradient(aba, xf, yf - 1, zf), Gradient(bba, xf - 1, yf - 1, zf),u);
	y1 = Lerp(x1, x2, v);

	x1 = Lerp(Gradient(aab, xf, yf, zf - 1) ,Gradient(bab, xf - 1, yf, zf - 1),u);

	x2 = Lerp(Gradient(abb, xf, yf - 1, zf - 1), Gradient(bbb, xf - 1, yf - 1, zf - 1),u);

	y2 = Lerp(x1, x2, v);

	return (Lerp(y1, y2, w) + 1) / 2;
}



float PerlinOctave(float x, float y, float z, float octave, float persistence)
{
	double total = 0;
	double frequency = 1;
	double amplitude = 1;
	double maxValue = 0;
	for (int i = 0;i < octave;i++) {
		total += Perlin(x * frequency, y * frequency, z * frequency) * amplitude;

		maxValue += amplitude;

		amplitude *= persistence;
		frequency *= 2;
	}

	return total / maxValue;
}

void PerlinDrawImgui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Perlin Worm Parameters")) {
		ImGui::SliderFloat("Scale", &m_params.scale, 1.f, 9.f);
		ImGui::SliderFloat("Threshold", &m_params.threshold, 0.0f, 1.0f);
		ImGui::SliderInt("Worm Count", &m_params.wormCount, 0, 20);
		ImGui::SliderInt("Worm Radius", &m_params.wormRadius, 1, 5);
		ImGui::SliderInt("Octave Count", &m_params.octave, 1, 10);
		ImGui::SliderFloat("Peristence", &m_params.octavePersistence, 0.1, 1.f);
		ImGui::TreePop();
	}
}


//void PerlinNoise::Update() {
//	if (!m_chunk) { return; }
//
//
//
//
//	for (int z = 0; z < m_depth; ++z) {
//		for (int y = 0; y < m_height; ++y) {
//			for (int x = 0; x < m_width; ++x) {
//				float nx = static_cast<float>(x) / m_width  * m_params.scale;
//				float ny = static_cast<float>(y) / m_height * m_params.scale;
//				float nz = static_cast<float>(z) / m_depth  * m_params.scale;
//
//				// Note:: glm::perlin returns [-1, 1] -> compute perlin noise value in [0, 1] 
//				float d = Perlin(nx, ny, nz ) * 0.5f + 0.5f;
//
//				// Carve if below threshold (geddit we carve Yh x Jaz on the tree)
//				m_chunk->at(x, y, z) = (d < m_params.threshold) ? EMPTY : SOLID;
//			}
//		}
//	}
//}
//
//void PerlinNoise::DrawImgui() {
//	if (ImGui::TreeNode("Perlin Noise Parameters")) {
//		ImGui::SliderFloat("Scale", &m_params.scale, 0.001f, 0.2f, "%.3f");
//		ImGui::SliderFloat("Threshold", &m_params.threshold, 0.0f, 1.0f);
//		ImGui::TreePop();
//	}
//}
//
//void PerlinNoise::SetChunk(std::shared_ptr<Chunks> chunk, const glm::vec3& worldMin, const glm::vec3& worldSize) {
//	m_chunk		= chunk;
//	m_worldMin	= worldMin;
//	m_worldSize = worldSize;
//	m_width		= chunk->GetWidth();
//	m_height	= chunk->GetHeight();
//	m_depth		= chunk->GetDepth();
//}
//
//void PerlinWorm::Update() {
//	if (!m_chunk) { return; }
//	ApplyPerlin();
//	CarveWorms();
//}
//
//void PerlinWorm::DrawImgui() {
//	if (ImGui::TreeNode("Perlin Worm Parameters")) {
//		ImGui::SliderFloat("Scale", &m_params.scale, 0.01f, 0.2f);
//		ImGui::SliderFloat("Threshold", &m_params.threshold, 0.0f, 1.0f);
//		ImGui::SliderInt("Worm Count", &m_params.wormCount, 0, 20);
//		ImGui::SliderInt("Worm Radius", &m_params.wormRadius, 1, 5);
//		ImGui::TreePop();
//	}
//}
//
//void PerlinWorm::StaticDrawImgui()
//{
//	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
//	if (ImGui::TreeNode("Perlin Worm Parameters")) {
//		ImGui::SliderFloat("Scale", &m_params.scale, 1.f, 9.f);
//		ImGui::SliderFloat("Threshold", &m_params.threshold, 0.0f, 1.0f);
//		ImGui::SliderInt("Worm Count", &m_params.wormCount, 0, 20);
//		ImGui::SliderInt("Worm Radius", &m_params.wormRadius, 1, 5);
//		ImGui::SliderInt("Octave Count", &m_params.octave, 1, 10);
//		ImGui::SliderFloat("Peristence", &m_params.octavePersistence, 0.1, 1.f);
//		ImGui::TreePop();
//	}
//}
//
//void PerlinWorm::SetChunk(std::shared_ptr<Chunks> chunk, const glm::vec3& worldMin, const glm::vec3& worldSize) {
//	m_chunk = chunk;
//	m_worldMin = worldMin;
//	m_worldSize = worldSize;
//	m_width = chunk->GetWidth();
//	m_height = chunk->GetHeight();
//	m_depth = chunk->GetDepth();
//}
//
//void PerlinWorm::ApplyPerlin(BlockType type) {
//
//
//	srand(m_seed);
//
//	float seedX = RandomClass::randomFloat(0.f, 100.f);
//	float seedY = RandomClass::randomFloat(0.f, 100.f);
//	float seedZ = RandomClass::randomFloat(0.f, 100.f);
//
//	for (int z = 0; z < m_depth; ++z) {
//		for (int y = 0; y < m_height; ++y) {
//			for (int x = 0; x < m_width; ++x) {
//				float nx = static_cast<float>(x) / m_width * m_params.scale + seedX;
//				float ny = static_cast<float>(y) / m_height * m_params.scale + seedY;
//				float nz = static_cast<float>(z) / m_depth * m_params.scale + seedZ;
//				float d = PerlinOctave(nx, ny, nz, m_params.octave, m_params.octavePersistence) * 0.5f + 0.5f;
//				if (d < m_params.threshold) {
//					m_chunk->at(x, y, z) = type;
//				}
//				
//			}
//		}
//	}
//}
//
//void PerlinWorm::CarveWorms() {
//	if (m_rooms.size() < 2) { return; }
//
//	std::mt19937 rng(std::random_device{}());
//	std::uniform_int_distribution<int> random{ 0, int(m_rooms.size() - 1) };
//
//	for (int i = 0; i < m_params.wormCount; ++i) {
//		const auto& a = m_rooms[random(rng)];
//		const auto& b = m_rooms[random(rng)];
//
//		glm::vec3 start = a.center + a.extent * 0.5f;
//		glm::vec3 end = b.center + b.extent * 0.5f;
//
//		glm::vec3 dir = glm::normalize(end - start);
//		float len = glm::length(end - start);
//
//		// walk from start to end, carving a sphere at each step
//		for (float t = 0; t < len; t += 1.0f) {
//			glm::vec3 p = start + dir * t;
//
//			// convert world-space to voxel-space
//			glm::vec3 norm = (p - m_worldMin) / m_worldSize;
//			glm::ivec3 cell = {
//				glm::clamp(int(norm.x * m_width),  0, m_width - 1),
//				glm::clamp(int(norm.y * m_height), 0, m_height - 1),
//				glm::clamp(int(norm.z * m_depth),  0, m_depth - 1)
//			};
//
//			// carve out a sphere of empty space at this cell cause why tf not
//			CarveSphere(cell, m_params.wormRadius);
//		}
//	}
//}
//
//// P.S. I saw this and thought it was cool so ill use it hehe
//// Carves a solid sphere of EMPTY blocks at the center
//void PerlinWorm::CarveSphere(const glm::ivec3& center, int radius) {
//	for (int z = -radius; z <= radius; ++z) {
//		for (int y = -radius; y <= radius; ++y) {
//			for (int x = -radius; x <= radius; ++x) {
//				glm::ivec3 p = center + glm::ivec3{ x, y, z };
//
//				// check if outofbounds if yes skip it baby
//				if (p.x < 0 || p.y < 0 || p.z < 0 ||
//					p.x >= m_width || p.y >= m_height || p.z >= m_depth) {
//					continue;
//				}
//
//				if (glm::length(glm::vec3{ x, y, z }) <= radius) {
//					m_chunk->at(p.x, p.y, p.z) = EMPTY;
//				}
//			}
//		}
//	}
//}
//
//
