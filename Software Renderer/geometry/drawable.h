#pragma once
#include <vector>
#include "geometry.h"

struct Drawable {
	Vec3 pos;
	float yaw = 0.0f, pitch = 0.0f, roll = 0.0f; // radians
	std::vector<Vec3> vertices = {};
	std::vector<uint32_t> triangles = {};
};