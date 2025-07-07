#include "Voxel.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <iostream>
#include <array>

namespace {
	constexpr int triTable[256][16] =
	{ {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
	{3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
	{3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
	{3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
	{9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
	{9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
	{2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
	{8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
	{9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
	{4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
	{3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
	{1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
	{4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
	{4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
	{5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
	{2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
	{9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
	{0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
	{2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
	{10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
	{5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
	{5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
	{9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
	{0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
	{1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
	{10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
	{8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
	{2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
	{7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
	{2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
	{11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
	{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
	{11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
	{11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
	{1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
	{9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
	{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
	{2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
	{5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
	{6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
	{3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
	{6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
	{5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
	{1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
	{10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
	{6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
	{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
	{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
	{3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
	{5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
	{0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
	{9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
	{8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
	{5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
	{0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
	{6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
	{10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
	{10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
	{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
	{1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
	{0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
	{10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
	{3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
	{6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
	{9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
	{8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
	{3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
	{6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
	{0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
	{10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
	{10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
	{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
	{7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
	{7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
	{2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
	{1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
	{11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
	{8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
	{0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
	{7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
	{10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
	{2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
	{6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
	{7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
	{2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
	{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
	{10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
	{10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
	{0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
	{7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
	{6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
	{8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
	{9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
	{6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
	{4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
	{10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
	{8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
	{0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
	{1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
	{8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
	{10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
	{4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
	{10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
	{5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
	{11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
	{9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
	{6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
	{7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
	{3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
	{7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
	{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
	{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
	{9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
	{1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
	{4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
	{7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
	{6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
	{3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
	{0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
	{6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
	{0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
	{11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
	{6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
	{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
	{9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
	{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
	{1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
	{10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
	{0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
	{5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
	{10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
	{11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
	{9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
	{7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
	{2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
	{8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
	{9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
	{9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
	{1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
	{9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
	{9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
	{5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
	{0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
	{10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
	{2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
	{0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
	{0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
	{9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
	{5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
	{3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
	{5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
	{8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
	{0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
	{9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
	{1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
	{3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
	{4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
	{9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
	{11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
	{11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
	{2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
	{9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
	{3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
	{1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
	{4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
	{3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
	{0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
	{9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
	{1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1} };



	constexpr int edgeTable[256] = {
		0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
		0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
		0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
		0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
		0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
		0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
		0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
		0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
		0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
		0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
		0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
		0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
		0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
		0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
		0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
		0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
		0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
		0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
		0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
		0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
		0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
		0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
		0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
		0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
		0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
		0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
		0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
		0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
		0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
		0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
		0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
		0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0 };

	constexpr int cornerIndexAFromEdge[12] = {
	0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3
	};

	constexpr int cornerIndexBFromEdge[12] = {
		1, 2, 3, 0, 5, 6, 7, 4, 4, 5, 6, 7
	};

	constexpr glm::vec3 cornerOffsets[8] = {
	glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 0, 1), glm::vec3(0, 0, 1),
	glm::vec3(0, 1, 0), glm::vec3(1, 1, 0), glm::vec3(1, 1, 1), glm::vec3(0, 1, 1)
	};
}

std::vector<Quad> Chunks::GenerateQuads() {
    std::vector<Quad> quads;

    // Process each face direction
    for (int face = 0; face < 6; face++) {
       // printf("Processing face %d\n", face);

        // For each face, we need to iterate through all blocks and check if that face should be rendered
        for (int x = 0; x < m_Width; x++) {
            for (int y = 0; y < m_Height; y++) {
                for (int z = 0; z < m_Depth; z++) {
                    if (shouldRenderFace(x, y, z, face)) {
                        // Calculate the actual position of this face in world space
                        Quad quad;
                        // Base position is the block position
                        int face_x = x;
                        int face_y = y;
                        int face_z = z;


                        // Apply world offset
                        quad.x = face_x;
                        quad.y = face_y;
                        quad.z = face_z;

                        quad.width = 1;
                        quad.height = 1;
                        quad.face = (Face)face;
                        quad.type = at(x, y, z);

                        //printf("Block (%d,%d,%d) Face %d -> Position (%d,%d,%d)\n",
                       //     x, y, z, face, quad.x, quad.y, quad.z);

                        quads.push_back(quad);
                    }
                }
            }
        }
    }

    //printf("Total quads generated: %zu\n", quads.size());
    return quads;
}

Voxel::Voxel(Shader& instancedShader, Shader& shader) :
	m_modified{ false },
	m_InstancedShader{ instancedShader },
	m_Shader{ shader }
{

    std::vector<GLfloat> verticesQuad = {
        // positions (x, y, z)
        -0.5f, -0.5f, 0.0f,  // bottom-left
         0.5f, -0.5f, 0.0f,  // bottom-right
         0.5f,  0.5f, 0.0f,  // top-right
        -0.5f,  0.5f, 0.0f   // top-left
    };

    // Indices for the two triangles that make up the quad
    std::vector<GLuint> indicesQuad = {
    0, 1, 2, // first triangle
    2, 3, 0  // second triangle
    };

	// Initialize with identity matrix, for mesh initialization
	m_transforms.push_back(glm::mat4(1.0f)); 


    m_InstancedQuad = std::make_unique<Mesh>(verticesQuad, indicesQuad, m_transforms.size(), m_transforms, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
	m_transforms.clear();
	m_InstancedQuad->GetInstanceVBO().UpdateData(m_transforms.data(), 0, 0);
}

void Voxel::UpdateChunk(int key)
{
	auto it = m_Chunks.find(key);
	if (it != m_Chunks.end()) {

		if (config.marchingCube) {
			// Generate vertices using Marching Cubes algorithm
			m_ChunkMarchingMesh[key] = it->second->GenerateVertices();
		}
		else {
			// Chunk exists, update it
		//MarchingCube test = it->second->GenerateVertices();
			std::vector<Quad> quads;
			if (config.greedy) {
				quads = it->second->GenerateQuadsGreedy();
			}
			else {
				quads = it->second->GenerateQuads();
			}

			std::vector<glm::mat4x4> transforms;

			float scaleFactor = it->second->GetScale(); // Scale factor for the cubes
			glm::vec3 chunkPosition = it->second->GetPosition();

			for (const auto& quad : quads) {

				//Chunk transformation
				glm::mat4 model = glm::mat4(1.f);
				model = glm::translate(model, chunkPosition);
				model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));


				glm::vec3 faceOffset;
				switch (quad.face) {
				case 0: faceOffset = glm::vec3(0.5f, 0.5f * (quad.height - 1), 0.5f * (quad.width - 1)); break; // +X
				case 1: faceOffset = glm::vec3(-0.5f, 0.5f * (quad.height - 1), 0.5f * (quad.width - 1)); break; // -X
				case 2: faceOffset = glm::vec3(0.5f * (quad.width - 1), 0.5f, 0.5f * (quad.height - 1)); break; // +Y
				case 3: faceOffset = glm::vec3(0.5f * (quad.width - 1), -0.5f, 0.5f * (quad.height - 1)); break; // -Y
				case 4: faceOffset = glm::vec3(0.5f * (quad.width - 1), 0.5f * (quad.height - 1), 0.5f); break; // +Z
				case 5: faceOffset = glm::vec3(0.5f * (quad.width - 1), 0.5f * (quad.height - 1), -0.5f); break; // -Z
				}


				model = glm::translate(model, glm::vec3(quad.x, quad.y, quad.z) + faceOffset);


				switch (quad.face) {
				case 0: // +X face (right)
					model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
					break;
				case 1: // -X face (left)
					model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
					break;
				case 2: // +Y face (top)
					model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
					break;
				case 3: // -Y face (bottom)
					model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));
					break;
				case 4: // +Z face (front)

					break;
				case 5: // -Z face (back)
					model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
					break;
				};

				model = glm::scale(model, glm::vec3(quad.width, quad.height, 1.f));

				transforms.emplace_back(model);
			}



			m_ChunkTransforms[key] = transforms;
		}

		

	}
	else {
		std::cout << "Chunk with key " << key << " does not exist." << std::endl;
        return;
		
	}
	m_modified = true;

}

void Voxel::UpdateAllChunk()
{
	for (const auto& chunk : m_Chunks) {
		UpdateChunk(chunk.first);
	}
}

void Voxel::DrawVoxel(const glm::mat4& vp, const glm::vec4& color, GLenum mode)
{
	if (config.marchingCube) {
		// Generate vertices using Marching Cubes algorithm
		m_Shader.Activate();
		GLsizei stride = 3 * sizeof(GLfloat);

		for (const auto& [key, mesh] : m_ChunkMarchingMesh) {
			VAO vao;
			VBO vbo(mesh.vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
			EBO ebo(mesh.indices);

			vao.Bind();
			vbo.Bind();
			ebo.Bind(); 


			vao.LinkAttribute(vbo, 0, 3, GL_FLOAT, stride, (void*)0);

			
			glm::mat4x4 model = vp * glm::translate(glm::mat4(1.0f), GetChunk(key)->GetPosition()) * glm::scale(glm::mat4(1.0f), glm::vec3(GetChunk(key)->GetScale()));

			m_Shader.setUniform("uniform_m2w", model);
			m_Shader.setUniform("uniform_color", color);


			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);

			//draw wireframe
			m_Shader.setUniform("uniform_color", glm::vec4{ 0.f,0.f,0.f,1.f });
			glEnable(GL_POLYGON_OFFSET_LINE);
			glPolygonOffset(-1.0f, -1.0f);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glLineWidth(1.0f);
			glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDisable(GL_POLYGON_OFFSET_LINE);

			

			vao.Delete();
			vbo.Delete();
			ebo.Delete();

		}

	}
	else {
		m_InstancedShader.Activate();
		//if modifed is true, updaet vbo
		if (m_modified) {
			//add all chunks transformation together
			m_transforms.clear();

			for (const auto& [key, transforms] : m_ChunkTransforms) {
				m_transforms.insert(m_transforms.end(), transforms.begin(), transforms.end());
			}

			m_InstancedQuad->SetInstanceCount(static_cast<GLuint>(m_transforms.size()));

			VAO& vao = m_InstancedQuad->GetVAO();
			VBO& instanceVBO = m_InstancedQuad->GetInstanceVBO();
			vao.Bind();
			instanceVBO.UpdateData(m_transforms.data(), m_transforms.size() * sizeof(glm::mat4x4), 0);

			m_modified = false;
		}

		if (m_transforms.empty()) return;

		m_InstancedShader.setUniform("uniform_m2w", vp);
		m_InstancedShader.setUniform("uniform_color", color);

		if (mode == GL_LINES) {

			//draw sold
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			m_InstancedQuad->Draw(GL_TRIANGLES);

			//draw wireframe
			m_InstancedShader.setUniform("uniform_color", glm::vec4{ 0.f,0.f,0.f,1.f });
			glEnable(GL_POLYGON_OFFSET_LINE);
			glPolygonOffset(-1.0f, -1.0f);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glLineWidth(1.0f);
			m_InstancedQuad->Draw(GL_TRIANGLES);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDisable(GL_POLYGON_OFFSET_LINE);
		}
		else {
			m_InstancedQuad->Draw(mode);
		}
	}
	
}

void Voxel::deleteChunk(int key)
{
	if (m_Chunks.find(key) != m_Chunks.end()) {
		m_Chunks.erase(key);
		m_ChunkTransforms.erase(key);
		m_ChunkMarchingMesh.erase(key);
		m_modified = true;
	}
	else {
		std::cout << "Chunk with key " << key << " does not exist." << std::endl;
	}
}

void Voxel::clearVoxel()
{
	std::vector<int> keysToDelete;
	for (const auto& chunk : m_Chunks) {
		keysToDelete.push_back(chunk.first);
	}

	for (int key : keysToDelete) {
		deleteChunk(key);
	}
}



std::vector<Quad> Chunks::GenerateQuadsGreedy()
{
	std::vector<Quad> quads;

	// Process each face direction
	for (int face = 0; face < 6; face++) {

		int depth_coord, plane_coord1, plane_coord2;
		int depth_size, plane_size1, plane_size2;

		switch (face) {
		case 0: 
		case 1: // +X, -X 
			depth_coord = 0; plane_coord1 = 1; plane_coord2 = 2;
			depth_size = m_Width; plane_size1 = m_Height; plane_size2 = m_Depth;
			break;
		case 2: 
		case 3: // +Y, -Y
			depth_coord = 1; plane_coord1 = 0; plane_coord2 = 2;
			depth_size = m_Height; plane_size1 = m_Width; plane_size2 = m_Depth;
			break;
		case 4: 
		case 5: // +Z, -Z 
			depth_coord = 2; plane_coord1 = 0; plane_coord2 = 1;
			depth_size = m_Depth; plane_size1 = m_Width; plane_size2 = m_Height;
			break;
		}


		for (int depth = 0; depth < depth_size; depth++) {
			// Create a mask for this slice
			std::vector<BlockType> mask(plane_size1 * plane_size2, EMPTY);

			// Fill the mask for this slice
			for (int p2 = 0; p2 < plane_size2; p2++) {
				for (int p1 = 0; p1 < plane_size1; p1++) {
					// Convert plane coordinates back to x,y,z
					int coords[3];
					coords[depth_coord] = depth;
					coords[plane_coord1] = p1;
					coords[plane_coord2] = p2;
					int x = coords[0], y = coords[1], z = coords[2];

					if (shouldRenderFace(x, y, z, face)) {
						mask[p1 + p2 * plane_size1] = at(x, y, z);
					}
					else {
						mask[p1 + p2 * plane_size1] = EMPTY;
					}
				}
			}

			// Generate quads from the mask using greedy meshing
			for (int p2 = 0; p2 < plane_size2; p2++) {
				for (int p1 = 0; p1 < plane_size1; ) {
					if (mask[p1 + p2 * plane_size1] != EMPTY) {
						BlockType blockType = mask[p1 + p2 * plane_size1];

						// Compute width (extend in p1 direction)
						int width = 1;
						while (p1 + width < plane_size1 &&
							mask[(p1 + width) + p2 * plane_size1] == blockType) {
							width++;
						}

						// Compute height (extend in p2 direction)
						int height = 1;
						bool canExtendHeight = true;
						while (p2 + height < plane_size2 && canExtendHeight) {
							// Check if the entire row can be extended
							for (int k = 0; k < width; k++) {
								if (mask[(p1 + k) + (p2 + height) * plane_size1] != blockType) {
									canExtendHeight = false;
									break;
								}
							}
							if (canExtendHeight) {
								height++;
							}
						}

						// Create the quad
						Quad quad;

						// Convert plane coordinates back to block coordinates
						int block_coords[3];
						block_coords[depth_coord] = depth;
						block_coords[plane_coord1] = p1;
						block_coords[plane_coord2] = p2;

						quad.x = block_coords[0];
						quad.y = block_coords[1];
						quad.z = block_coords[2];

						
						switch (face) {
						case 0: // +X face
						case 1: // -X face
							quad.width = height; 
							quad.height = width;  
							break;
						case 2: // +Y face
						case 3: // -Y face		
						case 4: // +Z face
						case 5: // -Z face
							quad.width = width;   
							quad.height = height; 
							break;
						}

						quad.face = (Face)face;
						quad.type = blockType;

						quads.push_back(quad);

						// Clear the mask for the area we just processed
						for (int h = 0; h < height; h++) {
							for (int w = 0; w < width; w++) {
								mask[(p1 + w) + (p2 + h) * plane_size1] = EMPTY;
							}
						}

						p1 += width;
					}
					else {
						p1++;
					}
				}
			}
		}
	}

	return quads;
}


MarchingCube Chunks::GenerateVertices()
{

	//For reference
	//      y
	 //     |
	 //     4--------5
	 //    /|       /|
	 //   7--------6 |
	 //   | |      | |
	 //   | 0------|-1 --> x
	 //   |/       |/
	 //   3--------2
	 //  /
	 //	z

	MarchingCube marchingCube;


	float isoLevel = 0.5f; // The isolevel for the marching cubes algorithm

	marchingCube.vertices.reserve(m_Width * m_Height * m_Depth * 8);
	marchingCube.vertices.reserve(m_Width * m_Height * m_Depth * 8);
	int countVertex = 0;

	// get the chunk pos based on the bottom left;
	glm::vec3 chunkSize = glm::vec3{ (float)GetWidth(), (float)GetHeight(), (float)GetDepth() };
	glm::vec3 pos = GetPosition();
	glm::vec3 chunkPos = GetPosition() - glm::vec3(0.5 * m_ScaleFactor) + (chunkSize / 2.f);

	for (int x = 0; x < m_Width; ++x) {
		for (int y = 0; y < m_Height; ++y) {
			for (int z = 0; z < m_Depth; ++z) {


				//if (x >= m_Width - 1 || y >= m_Height- 1 || z >= m_Depth - 1) {
				//	continue;
				//}


				//retrieve the corners of the cube

				std::array<glm::vec3, 8> cubeCorners;



				for (int i = 0; i < 8; ++i) {
					int cx = x + static_cast<int>(cornerOffsets[i].x);
					int cy = y + static_cast<int>(cornerOffsets[i].y);
					int cz = z + static_cast<int>(cornerOffsets[i].z);

				
					glm::vec3 localOffset = glm::vec3(cx, cy, cz);
					//cubeCorners[i] = chunkPos + localOffset * m_ScaleFactor;
					cubeCorners[i] = localOffset;
				}


				unsigned int lookUpIndex = 0;

				if (isSolid(x, y, z))         lookUpIndex |= 0b00000001; // Corner 0
				if (isSolid(x + 1, y, z))     lookUpIndex |= 0b00000010; // Corner 1
				if (isSolid(x + 1, y, z + 1)) lookUpIndex |= 0b00000100; // Corner 2
				if (isSolid(x, y, z + 1))     lookUpIndex |= 0b00001000; // Corner 3
				if (isSolid(x, y + 1, z))     lookUpIndex |= 0b00010000; // Corner 4
				if (isSolid(x + 1, y + 1, z)) lookUpIndex |= 0b00100000; // Corner 5
				if (isSolid(x + 1, y + 1, z + 1)) lookUpIndex |= 0b01000000; // Corner 6
				if (isSolid(x, y + 1, z + 1)) lookUpIndex |= 0b10000000; // Corner 7

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

std::optional<std::reference_wrapper<BlockType>> Chunks::GetWorldBlock(int x, int y, int z)
{
	int localX = x;
	int localY = y;
	int localZ = z;

	int chunkOffsetX = 0, chunkOffsetY = 0, chunkOffsetZ = 0;

	// Wrap coordinate into the local chunk or figure out neighbor chunk
	if (x < 0) {
		chunkOffsetX = -1;
		localX = m_Width + x;
	}
	else if (x >= m_Width) {
		chunkOffsetX = 1;
		localX = x - m_Width;
	}

	if (y < 0) {
		chunkOffsetY = -1;
		localY = m_Height + y;
	}
	else if (y >= m_Height) {
		chunkOffsetY = 1;
		localY = y - m_Height;
	}

	if (z < 0) {
		chunkOffsetZ = -1;
		localZ = m_Depth + z;
	}
	else if (z >= m_Depth) {
		chunkOffsetZ = 1;
		localZ = z - m_Depth;
	}

	if (chunkOffsetX == 0 && chunkOffsetY == 0 && chunkOffsetZ == 0) {
		return at(localX, localY, localZ);
	}

	if (m_Voxel) return std::nullopt;

	std::shared_ptr<Chunks> neighbor = m_Voxel->GetChunk( m_X + chunkOffsetX, m_Y + chunkOffsetY, m_Z + chunkOffsetZ);

	if (!neighbor) return std::nullopt;
	return std::ref(neighbor->at(localX, localY, localZ));
}

void Chunks::FillChunk(BlockType type)
{
	std::fill(m_Blocks.begin(), m_Blocks.end(), type); // Fill the entire chunk with the specified block type
}
