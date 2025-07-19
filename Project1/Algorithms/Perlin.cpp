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
	if (num >= 512) { return num % 512; }
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

	if (h < 4) { v = y; }
	else if (h == 12 || h == 14) { v = x; }
	else { v = z; }

	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float Perlin(float x, float y, float z) {
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
	x1 = Lerp(Gradient(aaa, xf, yf, zf), Gradient(baa, xf - 1, yf, zf), u);
	x2 = Lerp(Gradient(aba, xf, yf - 1, zf), Gradient(bba, xf - 1, yf - 1, zf), u);
	y1 = Lerp(x1, x2, v);
	x1 = Lerp(Gradient(aab, xf, yf, zf - 1), Gradient(bab, xf - 1, yf, zf - 1), u);
	x2 = Lerp(Gradient(abb, xf, yf - 1, zf - 1), Gradient(bbb, xf - 1, yf - 1, zf - 1), u);
	y2 = Lerp(x1, x2, v);

	return (Lerp(y1, y2, w) + 1) / 2;
}

float PerlinOctave(float x, float y, float z, float octave, float persistence) {
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

void PerlinDrawImgui() {
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Perlin Worm Parameters")) {
		ImGui::SliderFloat("Scale", &perlinParams.scale, 1.f, 9.f);
		ImGui::SliderFloat("Threshold", &perlinParams.threshold, 0.0f, 1.0f);
		ImGui::SliderInt("Octave Count", &perlinParams.octave, 1, 10);
		ImGui::SliderFloat("Peristence", &perlinParams.octavePersistence, 0.1, 1.f);
		ImGui::Checkbox("Generate Solid", &perlinParams.generateSolid);
		ImGui::TreePop();
	}
}