#pragma once
#include <cstdint>
#include <cmath>
#include <cassert>

template<int n> 
struct Vec;

template<> 
struct Vec<2> {
	union {
		struct { float x, y; };
		float data[2];
	};
	float& operator[](size_t i) { assert(i < 2); return data[i]; }
	const float& operator[](size_t i) const { assert(i < 2); return data[i]; }
	Vec<2>() : x(0), y(0) {}
	Vec<2>(float x, float y) : x(x), y(y) {}
};

template<> 
struct Vec<3> {
	union {
		struct { float x, y, z; };
		float data[3];
	};
	float& operator[](size_t i) { assert(i < 3); return data[i]; }
	const float& operator[](size_t i) const { assert(i < 3); return data[i]; }
	Vec<3>() : x(0), y(0), z(0) {}
	Vec<3>(float x, float y, float z) : x(x), y(y), z(z) {}
};

template<> 
struct Vec<4> {
	union {
		struct { float x, y, z, w; };
		float data[4];
	};
	float& operator[](size_t i) { assert(i < 4); return data[i]; }
	const float& operator[](size_t i) const { assert(i < 4); return data[i]; }
	Vec<4>(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	Vec<4>() : x(0), y(0), z(0), w(1) {}
};

template<int n> 
Vec<n> operator+(const Vec<n>& lhs, const Vec<n>& rhs) {
	Vec<n> out;
	for (int i = 0; i < n; i++) {
		out[i] = lhs[i] + rhs[i];
	}
	return out;
}

template<int n> 
Vec<n> operator-(const Vec<n>& lhs, const Vec<n>& rhs) {
	Vec<n> out;
	for (int i = 0; i < n; i++) {
		out[i] = lhs[i] - rhs[i];
	}
	return out;
}

template<int n> 
Vec<n> operator*(const Vec<n>& lhs, const float rhs) {
	Vec<n> out;
	for (int i = 0; i < n; i++) {
		out[i] = lhs[i] * rhs;
	}
	return out;
}

template<int n> 
Vec<n> operator*(const float lhs, const Vec<n>& rhs) {
	return rhs * lhs;
}

template<int n> 
Vec<n> operator/(const Vec<n>& lhs, const float rhs) {
	Vec<n> out;
	for (int i = 0; i < n; i++) {
		out[i] = lhs[i] / rhs;
	}
	return out;
}

template<int n>
float dot(const Vec<n>& a, const Vec<n>& b) {
	float sum = 0.0f;
	for (int i = 0; i < n; i++) {
		sum += a[i] * b[i];
	}
	return sum;
}

inline Vec<3> cross(const Vec<3>& a, const Vec<3>& b) {
	return Vec<3>(a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x);
}

inline float mag(const Vec<2>& v) {
	return sqrtf(v.x * v.x + v.y * v.y);
}

inline float mag(const Vec<3>& v) {
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

inline Vec<2> normalize(const Vec<2>& v) {
	float m = mag(v);
	assert(m != 0.0f);
	return v / m;
}

inline Vec<3> normalize(const Vec<3>& v) {
	float m = mag(v);
	assert(m != 0.0f);
	return v / m;
}

// Homogenous vectors
inline Vec<3> toCartesian(const Vec<4>& v) {
	assert(v.w != 0.0f);
	return Vec<3>(v.x / v.w, v.y / v.w, v.z / v.w);
}

typedef Vec<2> Vec2;
typedef Vec<3> Vec3;
typedef Vec<4> Vec4;