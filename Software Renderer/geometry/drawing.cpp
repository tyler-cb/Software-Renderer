#include "../state.h"
#include "../helpers.h"
#include "geometry.h"

#include <algorithm>

inline static Vec3 ndc_to_screen(const Vec3& v) {
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

// Draw a triangle ABC and shade based on provided texture. TODO: fragment/vertex shaders?
void draw_triangle( const Vec3& a, const Vec3& b, const Vec3& c,
					const Vec2& uv_a, const Vec2& uv_b, const Vec2& uv_c,
					const Texture& tex) {

	Vec2 a_xy = a.XY();
	Vec2 b_xy = b.XY();
	Vec2 c_xy = c.XY();

	Vec3 abc_max = max(a, b, c);
	Vec3 abc_min = min(a, b, c);
	float abc_area = signed_triangle_area(a_xy, b_xy, c_xy);

	// triangles facing away from the camera will have a negative area from signed_triangle_area.
	// therefore we can cull backside of faces like so
	if (abc_area < 1.0f) { return; }

	// Clamp the bounding box to the screen boundries.
	int x0 = std::max(0, static_cast<int>(std::floor(abc_min.x)));
	int x1 = std::min(WINDOW_WIDTH, static_cast<int>(std::ceil(abc_max.x)));
	int y0 = std::max(0, static_cast<int>(std::floor(abc_min.y)));
	int y1 = std::min(WINDOW_HEIGHT, static_cast<int>(std::ceil(abc_max.y)));

	if (x1 < x0 || y1 < y0) return;

	// We compute the barycentric coordinates of every pixel in the bounding box.
	// We can then determine if the pixel lies within the triangle.
#pragma omp parallel for
	for (int x = x0; x <= x1; x++) {
		for (int y = y0; y <= y1; y++) {

			if (x < 0 || x > WINDOW_WIDTH || y < 0 || y > WINDOW_HEIGHT) { continue; }

			float bary_a = signed_triangle_area(Vec2(x, y), b_xy, c_xy) / abc_area;
			float bary_b = signed_triangle_area(Vec2(x, y), c_xy, a_xy) / abc_area;
			float bary_c = signed_triangle_area(Vec2(x, y), a_xy, b_xy) / abc_area;

			if (bary_a < 0.0f || bary_b < 0.0f || bary_c < 0.0f) { continue; }
			if (state.debug_wireframe && !(bary_a < 0.01f || bary_b < 0.01f || bary_c < 0.01f)) { continue; }

			//Colour rainbow = Colour(bary_a * 255, bary_b * 255, bary_c * 255);

			float depth = bary_a * a.z + bary_b * b.z + bary_c * c.z;

			if (depth >= state.depth_buffer[y * WINDOW_WIDTH + x]) continue;
			Vec2 uv;
			uv.x = bary_a * uv_a.x + bary_b * uv_b.x + bary_c * uv_c.x;
			uv.y = bary_a * uv_a.y + bary_b * uv_b.y + bary_c * uv_c.y;

			Colour tex_colour = tex.sample_texture(uv);
			draw_pixel(x, y, tex_colour);
			draw_depth_pixel(x, y, depth);
		}
	}
}

// Draw a triangle ABC using colour col.
void draw_triangle(const Vec3& a, const Vec3& b, const Vec3& c, const Colour& col) {
	Vec2 a_xy = a.XY();
	Vec2 b_xy = b.XY();
	Vec2 c_xy = c.XY();

	Vec3 abc_max = max(a, b, c);
	Vec3 abc_min = min(a, b, c);
	float abc_area = signed_triangle_area(a_xy, b_xy, c_xy);

	// triangles facing away from the camera will have a negative area from signed_triangle_area.
	// therefore we can cull backside of faces like so
	if (abc_area < 1.0f) { return; }

	// Clamp the bounding box to the screen boundries.
	int x0 = std::max(0, static_cast<int>(std::floor(abc_min.x)));
	int x1 = std::min(WINDOW_WIDTH, static_cast<int>(std::ceil(abc_max.x)));
	int y0 = std::max(0, static_cast<int>(std::floor(abc_min.y)));
	int y1 = std::min(WINDOW_HEIGHT, static_cast<int>(std::ceil(abc_max.y)));

	if (x1 < x0 || y1 < y0) return;

	// We compute the barycentric coordinates of every pixel in the bounding box.
	// We can then determine if the pixel lies within the triangle.
#pragma omp parallel for
	for (int x = x0; x <= x1; x++) {
		for (int y = y0; y <= y1; y++) {

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
	Texture* drawable_texture;

	for (const Drawable& d : state.drawables) {

		if (d.texture != nullptr) {
			drawable_texture = d.texture;
		}
		else {
			drawable_texture = &state.debug_texture;
		}

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

			//draw_triangle(v1_screen, v2_screen, v3_screen, random_colour(t.a));
			draw_triangle(v1_screen, v2_screen, v3_screen,
				d.vertices[t.a].uv, d.vertices[t.b].uv, d.vertices[t.c].uv,
				*drawable_texture);
		}
	}
}