#include "RandomAlgo.h"
#include <random>



std::vector<bool> RandomClass::GenerateMinTrue(unsigned int seed, unsigned int minTrue, unsigned int maxSize)
{
	if (minTrue > maxSize ) {
		minTrue = maxSize;
	}


	std::vector<bool> results(maxSize, false);
	std::mt19937 rng(seed);


	// First, set 3 guaranteed trues
	for (int i = 0; i < minTrue; ++i) {
		results[i] = true;
	}

	// Fill the remaining 7 with random true/false
	std::uniform_int_distribution<int> dist(0, 1);

	for (int i = minTrue; i < maxSize; ++i) {
		results[i] = dist(rng); // true or false
	}

	// Shuffle the result so guaranteed trues aren't always at the start
	std::shuffle(results.begin(), results.end(), rng);

	return results;
}

std::vector<bool> RandomClass::GenerateMaxTrue(unsigned int seed, unsigned int maxTrue, unsigned int maxSize)
{
	if (maxTrue > maxSize) {
		maxTrue = maxSize;
	}


	std::vector<bool> results(maxSize, false);
	std::mt19937 rng(seed);


	// First, set 3 guaranteed trues
	for (int i = 0; i < maxTrue; ++i) {
		results[i] = true;
	}


	// Shuffle the result so guaranteed trues aren't always at the start
	std::shuffle(results.begin(), results.end(), rng);

	return results;
}
