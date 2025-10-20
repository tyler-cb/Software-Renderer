#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <tuple>

#include "geometry.h"
#include "transformations.h"

struct Drawable {
	std::string name = "";
	Vec3 pos;
	float yaw = 0.0f, pitch = 0.0f, roll = 0.0f; // radians
	std::vector<Vertex> vertices = {};
	std::vector<Triangle> triangles = {};

	// translation + rotation;
	Mat4 model_matrix;

	Drawable() {
		update_model_matrix();
	}

	void update_model_matrix() {
		Mat4 trans = make_translation_matrix(pos.x, pos.y, pos.z);
		Mat4 rot = make_rotation_matrix(pitch, yaw, roll);

		model_matrix = trans * rot;
	}

	Vec3 get_vertex_world_coord(const int& n) const {
		assert(n >= 0 && n < vertices.size());
		return toCartesian(model_matrix * Vec4(vertices[n].pos, 1.0f));
	}
};



Drawable import_from_obj(const std::string& filepath);