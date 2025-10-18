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
		struct { float u, v; };
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
	Vec<4>(const Vec<3>& v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}
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

inline Vec<2> max(const Vec<2>& a, const Vec<2>& b, const Vec<2>& c) {
	Vec<2> v;
	v.x = std::fmax(a.x, std::fmax(b.x, c.x));
	v.y = std::fmax(a.y, std::fmax(b.y, c.y));
	return v;
}

inline Vec<3> max(const Vec<3>& a, const Vec<3>& b, const Vec<3>& c) {
	Vec<3> v;
	v.x = std::fmax(a.x, std::fmax(b.x, c.x));
	v.y = std::fmax(a.y, std::fmax(b.y, c.y));
	v.z = std::fmax(a.z, std::fmax(b.z, c.z));
	return v;
}

inline Vec<2> min(const Vec<2>& a, const Vec<2>& b, const Vec<2>& c) {
	Vec<2> v;
	v.x = std::fmin(a.x, std::fmin(b.x, c.x));
	v.y = std::fmin(a.y, std::fmin(b.y, c.y));
	return v;
}

inline Vec<3> min(const Vec<3>& a, const Vec<3>& b, const Vec<3>& c) {
	Vec<3> v;
	v.x = std::fmin(a.x, std::fmin(b.x, c.x));
	v.y = std::fmin(a.y, std::fmin(b.y, c.y));
	v.z = std::fmin(a.z, std::fmin(b.z, c.z));
	return v;
}

// The area is signed depending on if ABC is clockwise or cc.
// counter-clockwise = +ve, clockwise = -ve
inline float signedTriangleArea(const Vec<2>& a, const Vec<2>& b, const Vec<2>& c) {
	return 0.5f * ((b.y - a.y) * (b.x + a.x) + (c.y - b.y) * (c.x + b.x) + (a.y - c.y) * (a.x + c.x));
}

typedef Vec<2> Vec2;
typedef Vec<3> Vec3;
typedef Vec<4> Vec4;

#define WORLD_FORWARD Vec3(0.0f, 0.0f, 1.0f)
#define WORLD_UP Vec3(0.0f, 1.0f, 0.0f)
#define WORLD_RIGHT Vec3(1.0f, 0.0f, 0.0f)

template <int rows, int cols>
struct Mat {
	float data[rows][cols];

	Mat() {
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				data[i][j] = 0.0f;
			}
		}
	}

	float& operator()(int row, int col) { return data[row][col]; }
	const float& operator()(int row, int col) const { return data[row][col]; }
};

template<int rows_lhs, int cols_lhs, int cols_rhs>
Mat<rows_lhs, cols_rhs> operator*(const Mat<rows_lhs, cols_lhs>& lhs, const Mat<cols_lhs, cols_rhs>& rhs) {
    Mat<rows_lhs, cols_rhs> result;

    for (int i = 0; i < rows_lhs; i++) {
        for (int j = 0; j < cols_rhs; j++) {
            float sum = 0.0f;
            for (int k = 0; k < cols_lhs; k++) {
                sum += lhs.data[i][k] * rhs.data[k][j];
            }
            result.data[i][j] = sum;
        }
    }
    return result;
}

template<int n>
Vec<n> operator*(const Mat<n, n>& matrix, const Vec<n>& vector) {
	Vec<n> result;

	for (int i = 0; i < n; i++) {
		float sum = 0.0f;
		for (int j = 0; j < n; j++) {
			sum += matrix.data[i][j] * vector[j];
		}
		result[i] = sum;
	}

	return result;
}

template <int n>
Mat<n, n> transpose(const Mat<n, n> matrix) {
	Mat<n, n> out;

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			out.data[i][j] = matrix[j][i];
		}
	}
}

typedef Mat<3, 3> Mat3;
typedef Mat<4, 4> Mat4;

struct Vertex {
	Vec3 pos;
	Vec2 uv;
	Vec3 normal;
};

struct Triangle {
	union {
		struct {
			uint32_t a;
			uint32_t b;
			uint32_t c;
		};
		uint32_t indices[3];
	};
};