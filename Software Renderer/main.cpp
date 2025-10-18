#pragma once

#include <cstdint>
#include <chrono>
#include <iostream>

#include "SDL3/SDL.h"

#include "state.h"
#include "helpers.h"
#include "geometry/drawing.h"
#include "geometry/geometry.h"

State state;
Frametimes frametimes;
bool running = true;

static void importAllObj();

int main(int argc, int8_t argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	state.window = SDL_CreateWindow("Software Renderer",
		WINDOW_WIDTH, WINDOW_HEIGHT, 0);

	state.renderer = SDL_CreateRenderer(state.window, nullptr);
	state.texture = SDL_CreateTexture(state.renderer,
		SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);

	double frametime = 0.0;

	importAllObj();
	std::cout << "Importing all objs took " << state.last_frame_time << "ms\n";

	while (running) {
		while (SDL_PollEvent(&state.event)) {
			if (state.event.type == SDL_EVENT_QUIT) {
				running = false;
			}
		}

		draw_frame();
		frametimes.add(state.last_frame_time);

#ifdef _DEBUG
		if (frametimes.frame_number % 20 == 0) {
			std::cout << "Frame time: " << frametimes.averageMs() << "ms\n";
			std::cout << "FPS: " << frametimes.averageFps() << "\n";
		}
#endif

		SDL_UpdateTexture(state.texture, nullptr, state.pixels, WINDOW_WIDTH * sizeof(uint32_t));
		SDL_RenderClear(state.renderer);
		SDL_RenderTexture(state.renderer, state.texture, NULL, NULL);
		SDL_RenderPresent(state.renderer);
	}

	SDL_DestroyTexture(state.texture);
	SDL_DestroyRenderer(state.renderer);
	SDL_DestroyWindow(state.window);
	SDL_Quit();
	return 0;
}

static void importAllObj() {
	Autotimer timer(&state.last_frame_time);
	state.drawables.push_back(importFromObj("../TiltedCube.obj"));
}