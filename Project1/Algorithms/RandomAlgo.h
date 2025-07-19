#pragma once

#include <vector>

namespace rdm {
	std::vector<bool> GenerateMinTrue(unsigned int seed, unsigned int minTrue, unsigned int maxSize);
	std::vector<bool> GenerateMaxTrue(unsigned int seed, unsigned int maxTrue, unsigned int maxSize);

	float RandomFloat(float min, float max);
}