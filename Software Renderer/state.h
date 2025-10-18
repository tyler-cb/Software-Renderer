#pragma once

#include <vector>

#include "SDL3/SDL.h"
#include "geometry/geometry.h"
#include "geometry/drawable.h"
#include "geometry/camera.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

// global state
struct State {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	SDL_Event event;
	uint32_t pixels[WINDOW_WIDTH * WINDOW_HEIGHT];
	std::vector<Drawable> drawables = {};
	Camera camera = Camera(Vec3(0.0f, 0.0f, 0.0f));
	double last_frame_time;
};

extern State state;