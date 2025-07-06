#pragma once
#include <vector>

class RandomClass
{
public:
	static std::vector<bool> GenerateMinTrue(unsigned int seed, unsigned int minTrue, unsigned int maxSize);
	static std::vector<bool> GenerateMaxTrue(unsigned int seed, unsigned int maxTrue, unsigned int maxSize);
};

