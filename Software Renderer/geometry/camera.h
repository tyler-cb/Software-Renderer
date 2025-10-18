#pragma once

#include "transformations.h"
#include "../helpers.h"

enum ProjectionType {
	Orthographic,
	Perspective
};

struct Camera {
	ProjectionType projection;
	Vec3 pos;
	float yaw, pitch, roll = 0.0f; // radians
	float vertical_fov, horizontal_fov = DegToRad(90.0); // radians
	float near_plane, far_plane;
	float aspect_ratio = 16.0f / 9.0f;

	// only used for orthographic projection, how many units wide/high the camera can see
	float ortho_width, ortho_height;

	// store orientation vectors that only update when the yaw, pitch, roll changes.
	Vec3 forward, right, up;

	Mat4 viewMatrix;
	Mat4 projectionMatrix;

	Camera(Vec3 pos) {
		this->pos = pos;
		ComputeOrientation();
	}

	void ComputeOrientation() {
		Mat3 R = makeRotationMatrix(-pitch, yaw, roll);

		forward = R * WORLD_FORWARD;
		right = R * WORLD_RIGHT;
		up = R * WORLD_UP;
	}
};