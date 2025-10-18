#pragma once

#include "transformations.h"
#include "../helpers.h"

enum Projection {
	Orthographic,
	Perspective
};

struct Camera {
	Projection projection = Orthographic;
	Vec3 pos;

	float yaw = 0.0f, pitch = 0.0f, roll = 0.0f; // radians

	float vertical_fov = DegToRad(90.0), horizontal_fov; // radians
	float near_plane = 0.1f;
	float far_plane = 100.0f;
	float aspect_ratio = 16.0f / 9.0f;

	// only used for orthographic projection, how many units wide/high the camera can see
	float ortho_width = 20.0f, ortho_height = 20.0f;

	// store orientation vectors that only update when the yaw, pitch, roll changes.
	Vec4 forward, right, up;

	// View transform - places the camera at the origin and rotates the world relative
	// to the camera. The inverse of the camera's position and rotation in world space.
	Mat4 viewMatrix; 

	// Projection transform -
	// Transform the geometry based on perspective (or not if its orthographic)
	Mat4 projectionMatrix;

	Camera(const Vec3& pos) : pos(pos) {
		horizontal_fov = 2.0f * atanf(tanf(vertical_fov * 0.5f) * aspect_ratio);
		ComputeOrientation();
	}

	void ComputeOrientation() {
		Mat4 r = makeRotationMatrix(-pitch, yaw, roll);

		forward = r * Vec4(WORLD_FORWARD, 0.0f);
		right = r * Vec4(WORLD_RIGHT, 0.0f);
		up = r * Vec4(WORLD_UP, 0.0f);

		updateViewMatrix();
	}

	void updateViewMatrix() {
		
		viewMatrix(0, 0) = right.x; viewMatrix(0, 1) = up.x; viewMatrix(0, 2) = -forward.x;
		viewMatrix(1, 0) = right.y; viewMatrix(1, 1) = up.y; viewMatrix(1, 2) = -forward.y;
		viewMatrix(2, 0) = right.z; viewMatrix(2, 1) = up.z; viewMatrix(2, 2) = -forward.z;

		viewMatrix(0, 3) = -dot(right, Vec4(pos, 0));
		viewMatrix(1, 3) = -dot(up, Vec4(pos, 0));
		viewMatrix(2, 3) = dot(forward, Vec4(pos, 0));
		viewMatrix(3, 3) = 1.0f;
	}

	void updateProjectionMatrix() {

	}

};