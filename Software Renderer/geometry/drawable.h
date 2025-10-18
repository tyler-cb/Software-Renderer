#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <tuple>

#include "geometry.h"

struct Drawable {
	std::string name = "";
	Vec3 pos;
	float yaw = 0.0f, pitch = 0.0f, roll = 0.0f; // radians
	std::vector<Vertex> vertices = {};
	std::vector<Triangle> triangles = {};
};

Drawable importFromObj(const std::string& filepath);