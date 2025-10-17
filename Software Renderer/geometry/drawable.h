#pragma once
#include <vector>
#include "geometry.h"

struct Drawable {
	std::vector<Vec3> vertices = {};
	std::vector<uint32_t> triangles = {};
};