#pragma once

#include "SDL3/SDL.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

struct State {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	SDL_Event event;
	uint32_t pixels[WINDOW_WIDTH * WINDOW_HEIGHT];
};

extern State state;