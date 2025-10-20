#include "../state.h"
#include "../helpers.h"
#include "geometry.h"

inline Vec3 ndc_to_screen(const Vec3& v) {
	Vec3 screen;
	screen.x = (v.x * 0.5f + 0.5f) * WINDOW_WIDTH;
	screen.y = (1.0f - (v.y * 0.5f + 0.5f)) * WINDOW_HEIGHT;
	screen.z = (v.z * 0.5f + 0.5f);
	return screen;
}

void draw_pixel(const int& x, const int& y, Colour c) {
	if (x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT) return;
	state.pixels[y * WINDOW_WIDTH + x] = c.c;
}

void draw_depth_pixel(const int& x, const int& y, const float& depth) {
	if (x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT || depth < 0.0f || depth > 1.0f) return;
	state.depth_buffer[y * WINDOW_WIDTH + x] = depth;
}

// Draw a triangle ABC using colour col.
void draw_triangle(const Vec3& a, const Vec3& b, const Vec3& c, const Colour& col) {
	Vec2 a_xy = a.XY();
	Vec2 b_xy = b.XY();
	Vec2 c_xy = c.XY();

	Vec3 abc_max = max(a, b, c);
	Vec3 abc_min = min(a, b, c);
	float abc_area = signed_triangle_area(a_xy, b_xy, c_xy);

	// We compute the barycentric coordinates of every pixel in the bounding box.
	// We can then determine if the pixel lies within the triangle.
#pragma omp parallel for
	for (int x = abc_min.x; x <= abc_max.x; x++) {
		for (int y = abc_min.y; y <= abc_max.y; y++) {

			if (x < 0 || x > WINDOW_WIDTH || y < 0 || y > WINDOW_HEIGHT) { continue; }

			float bary_a = signed_triangle_area(Vec2(x, y), b_xy, c_xy) / abc_area;
			float bary_b = signed_triangle_area(Vec2(x, y), c_xy, a_xy) / abc_area;
			float bary_c = signed_triangle_area(Vec2(x, y), a_xy, b_xy) / abc_area;

			if (bary_a < 0.0f || bary_b < 0.0f || bary_c < 0.0f) { continue; }
			if (state.debug_wireframe && !(bary_a < 0.01f || bary_b < 0.01f || bary_c < 0.01f)) { continue; }

			//Colour rainbow = Colour(bary_a * 255, bary_b * 255, bary_c * 255);

			float depth = bary_a * a.z + bary_b * b.z + bary_c * c.z;

			if (depth >= state.depth_buffer[y * WINDOW_WIDTH + x]) continue;

			draw_pixel(x, y, col);
			draw_depth_pixel(x, y, depth);
		}
	}
}

bool outside_ndc(const Vec3& v) {
	return (v.x < -1.0f || v.x > 1.0f ||
		v.y < -1.0f || v.y > 1.0f ||
		v.z < -1.0f || v.z > 1.0f);
}

void draw_frame() {
	Autotimer timer(&state.last_frame_time);

	std::memset(state.pixels, 0, sizeof(state.pixels));
	std::fill_n(state.depth_buffer, WINDOW_HEIGHT * WINDOW_WIDTH, 1.0f);

	for (const Drawable& d : state.drawables) {
		for (const Triangle& t : d.triangles) {
			Vec4 v1_clip = state.camera.project(d.get_vertex_world_coord(t.a));
			Vec4 v2_clip = state.camera.project(d.get_vertex_world_coord(t.b));
			Vec4 v3_clip = state.camera.project(d.get_vertex_world_coord(t.c));

			// negative w means behind the camera; so skip.
			if (v1_clip.w <= 0.0f || v2_clip.w <= 0.0f || v3_clip.w <= 0.0f) {
				continue;
			}

			Vec3 v1_ndc = toCartesian(v1_clip);
			Vec3 v2_ndc = toCartesian(v2_clip);
			Vec3 v3_ndc = toCartesian(v3_clip);
			
			if (outside_ndc(v1_ndc) && outside_ndc(v2_ndc) && outside_ndc(v3_ndc)) {
				continue;
			}

			Vec3 v1_screen = ndc_to_screen(v1_ndc);
			Vec3 v2_screen = ndc_to_screen(v2_ndc);
			Vec3 v3_screen = ndc_to_screen(v3_ndc);

			draw_triangle(v1_screen, v2_screen, v3_screen, random_colour(t.a));
		}
	}
}