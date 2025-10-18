#pragma once

#include "geometry.h"

inline Mat3 makeRotationMatrix(float x, float y, float z) {
	// pitch = rotation about x.
	// yaw = rotation about y.
	// roll = rotation about z.

	float sinx = sinf(x);
	float cosx = cosf(x);
	float siny = sinf(y);
	float cosy = cosf(y);
	float sinz = sinf(z);
	float cosz = cosf(z);

	// we will create the 3 rotation matrices for each axis and then multiply them
	Mat3 Rx, Ry, Rz;

	// Rx
	Rx(0, 0) = 1.0f; Rx(0, 1) = 0.0f;  Rx(0, 2) = 0.0f;
	Rx(1, 0) = 0.0f; Rx(1, 1) = cosx;  Rx(1, 2) = -sinx;
	Rx(2, 0) = 0.0f; Rx(2, 1) = sinx;  Rx(2, 2) = cosx;

	// Ry
	Ry(0, 0) = cosy;  Ry(0, 1) = 0.0f; Ry(0, 2) = siny;
	Ry(1, 0) = 0.0f;  Ry(1, 1) = 1.0f; Ry(1, 2) = 0.0f;
	Ry(2, 0) = -siny; Ry(2, 1) = 0.0f; Ry(2, 2) = cosy;

	// Rz
	Rz(0, 0) = cosz; Rz(0, 1) = -sinz; Rz(0, 2) = 0.0f;
	Rz(1, 0) = sinz; Rz(1, 1) = cosz;  Rz(1, 2) = 0.0f;
	Rz(2, 0) = 0.0f; Rz(2, 1) = 0.0f;  Rz(2, 2) = 1.0f;

	// ZXY order
	return Ry * Rx * Rz;
}