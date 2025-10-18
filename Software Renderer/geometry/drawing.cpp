#include "../state.h"
#include "../helpers.h"
#include "geometry.h"

void draw_pixel(const int x, const int y, Colour c) {
	state.pixels[y * WINDOW_WIDTH + x] = c.c;
}

// Draw a triangle ABC using colour col.
void draw_triangle(const Vec2& a, const Vec2& b, const Vec2& c, Colour col) {
	Vec2 abc_max = max(a, b, c);
	Vec2 abc_min = min(a, b, c);
	float abc_area = signed_triangle_area(a, b, c);

	// We compute the barycentric coordinates of every pixel in the bounding box.
	// We can then determine if the pixel lies within the triangle.
#pragma omp parallel for
	for (int x = abc_min.x; x <= abc_max.x; x++) {
		for (int y = abc_min.y; y <= abc_max.y; y++) {
			float bary_a = signed_triangle_area(Vec2(x, y), b, c) / abc_area;
			float bary_b = signed_triangle_area(Vec2(x, y), c, a) / abc_area;
			float bary_c = signed_triangle_area(Vec2(x, y), a, b) / abc_area;
			if (bary_a < 0.0f || bary_b < 0.0f || bary_c < 0.0f) { continue; }
			//Colour rainbow = Colour(bary_a * 255, bary_b * 255, bary_c * 255);
			draw_pixel(x, y, col);
		}
	}
}

// Draw a wireframe ABC using colour col.
void draw_wireframe(const Vec2& a, const Vec2& b, const Vec2& c, Colour col) {
	Vec2 abc_max = max(a, b, c);
	Vec2 abc_min = min(a, b, c);
	float abc_area = signed_triangle_area(a, b, c);

	// We use the barycentric coordinates again but only draw when any barycentric
	// coordinate is close to 0, meaning close to the edge of the triangle.
#pragma omp parallel for
	for (int x = abc_min.x; x <= abc_max.x; x++) {
		for (int y = abc_min.y; y <= abc_max.y; y++) {
			float bary_a = signed_triangle_area(Vec2(x, y), b, c) / abc_area;
			float bary_b = signed_triangle_area(Vec2(x, y), c, a) / abc_area;
			float bary_c = signed_triangle_area(Vec2(x, y), a, b) / abc_area;
			if (bary_a < 0.0f || bary_b < 0.0f || bary_c < 0.0f) { continue; }
			if (!(bary_a < 0.01f || bary_b < 0.01f || bary_c < 0.01f)) { continue; }
			//Colour rainbow = Colour(bary_a * 255, bary_b * 255, bary_c * 255);
			draw_pixel(x, y, col);
		}
	}
}

void draw_frame() {
	Autotimer timer(&state.last_frame_time);
	draw_wireframe(Vec2(500, 500), Vec2(500, 700), Vec2(700, 500), Colour(0, 255, 0, 255));
	for (Drawable d : state.drawables) {
		for (Triangle t : d.triangles) {
			Vec2 v1, v2, v3;

			v1 = state.camera.project(d.vertices[t.a].pos);
			v2 = state.camera.project(d.vertices[t.b].pos);
			v3 = state.camera.project(d.vertices[t.c].pos);

			draw_wireframe(v1, v2, v3, random_colour());
		}
	}
}