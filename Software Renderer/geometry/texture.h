#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <cmath>

#include "stb_image.h"

#include "../helpers.h"
#include "geometry.h"

struct Texture {
	int width = 0;
	int height = 0;
	int channels = 4; // ARGB
	std::vector<uint8_t> pixels;

	Colour sample_texture(const Vec2& uv) const {
		if (width <= 0 || height <= 0 || pixels.empty()) return Colour(0, 0, 0, 255);

		float u = uv.x - std::floor(uv.x);
		float v = uv.y - std::floor(uv.y);

		// Using point filtering for now (closest pixel)
		// TODO: bilinear filtering
		int x = static_cast<int>(u * width) % width;
		int y = static_cast<int>(v * height) % height;

		int index = (y * width + x) * channels;

		return Colour(
			pixels[index + 0],
			pixels[index + 1],
			pixels[index + 2],
			channels == 4 ? pixels[index + 3] : 255);
	}
};

// assume the file has 32 bit pixel depth
inline Texture load_tga(const std::string& path) {
	Texture out;
	int w = 0, h = 0, channels = 0;
	uint8_t* data = stbi_load(path.c_str(), &w, &h, &channels, 4);
	if (!data) return out;

	out.width = w;
	out.height = h;
	const uint64_t pixel_count = static_cast<uint64_t>(w) * static_cast<uint64_t>(h);
	out.pixels.resize(pixel_count * channels);
	for (uint64_t i = 0; i < pixel_count; i++) {
		out.pixels[i * 4 + 0] = data[i * 4 + 0];
		out.pixels[i * 4 + 1] = data[i * 4 + 1];
		out.pixels[i * 4 + 2] = data[i * 4 + 2];
		out.pixels[i * 4 + 3] = data[i * 4 + 3];
	}

	stbi_image_free(data);
	return out;
}