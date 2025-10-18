#include "../state.h"
#include "../helpers.h"
#include "geometry.h"

void drawPixel(const int x, const int y, Colour c) {
	state.pixels[y * WINDOW_WIDTH + x] = c.c;
}

// Draw a triangle ABC using colour col.
void drawTriangle(const Vec2& a, const Vec2& b, const Vec2& c, Colour col) {
	Vec2 abc_max = max(a, b, c);
	Vec2 abc_min = min(a, b, c);
	float abc_area = signedTriangleArea(a, b, c);

	// We compute the barycentric coordinates of every pixel in the bounding box.
	// We can then determine if the pixel lies within the triangle.
#pragma omp parallel for
	for (int x = abc_min.x; x <= abc_max.x; x++) {
		for (int y = abc_min.y; y <= abc_max.y; y++) {
			float bary_a = signedTriangleArea(Vec2(x, y), b, c) / abc_area;
			float bary_b = signedTriangleArea(Vec2(x, y), c, a) / abc_area;
			float bary_c = signedTriangleArea(Vec2(x, y), a, b) / abc_area;
			if (bary_a < 0.0f || bary_b < 0.0f || bary_c < 0.0f) { continue; }
			//Colour rainbow = Colour(bary_a * 255, bary_b * 255, bary_c * 255);
			drawPixel(x, y, col);
		}
	}
}

// Draw a wireframe ABC using colour col.
void drawWireframe(const Vec2& a, const Vec2& b, const Vec2& c, Colour col) {
	Vec2 abc_max = max(a, b, c);
	Vec2 abc_min = min(a, b, c);
	float abc_area = signedTriangleArea(a, b, c);

	// We use the barycentric coordinates again but only draw when any barycentric
	// coordinate is close to 0, meaning close to the edge of the triangle.
#pragma omp parallel for
	for (int x = abc_min.x; x <= abc_max.x; x++) {
		for (int y = abc_min.y; y <= abc_max.y; y++) {
			float bary_a = signedTriangleArea(Vec2(x, y), b, c) / abc_area;
			float bary_b = signedTriangleArea(Vec2(x, y), c, a) / abc_area;
			float bary_c = signedTriangleArea(Vec2(x, y), a, b) / abc_area;
			if (bary_a < 0.0f || bary_b < 0.0f || bary_c < 0.0f) { continue; }
			if (!(bary_a < 0.01f || bary_b < 0.01f || bary_c < 0.01f)) { continue; }
			//Colour rainbow = Colour(bary_a * 255, bary_b * 255, bary_c * 255);
			drawPixel(x, y, col);
		}
	}
}

void draw_frame() {
	Autotimer timer(&state.last_frame_time);
	drawWireframe(Vec2(500, 500), Vec2(500, 700), Vec2(700, 500), Colour(0, 255, 0, 255));
}