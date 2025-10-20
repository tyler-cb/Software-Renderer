#pragma once

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;

#include <vector>

#include "SDL3/SDL.h"
#include "geometry/geometry.h"
#include "geometry/drawable.h"
#include "geometry/camera.h"

struct Drawable;

// global state
struct State {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	SDL_Event event;
	uint32_t pixels[WINDOW_WIDTH * WINDOW_HEIGHT];
	float depth_buffer[WINDOW_WIDTH * WINDOW_HEIGHT];
	std::vector<Drawable> drawables = {};
	Camera camera = Camera(Vec3(0.0f, 0.0f, 0.0f));
	double last_frame_time;

	// when true, show the depth buffer on screen.
	bool debug_depth = false;
	// when true, draw wireframes rather than filled triangles.
	bool debug_wireframe = false;
};

extern State state;