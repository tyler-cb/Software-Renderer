#pragma once

#include <cstdint>
#include <chrono>
#include <iostream>

#include "SDL3/SDL.h"

#include "state.h"
#include "helpers.h"

State state;
bool running = true;

int main(int argc, int8_t argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	state.window = SDL_CreateWindow("Software Renderer",
		WINDOW_WIDTH, WINDOW_HEIGHT, 0);

	state.renderer = SDL_CreateRenderer(state.window, nullptr);
	state.texture = SDL_CreateTexture(state.renderer,
		SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);

	double frametime = 0.0;
	while (running) {
		while (SDL_PollEvent(&state.event)) {
			if (state.event.type == SDL_EVENT_QUIT) {
				running = false;
			}
		}

		{
			autotimer timer(&frametime);
#pragma omp parallel for
			for (int y = 0, i = 0; y < WINDOW_HEIGHT; y++) {
				for (int x = 0; x < WINDOW_WIDTH; x++, i++) {
					state.pixels[i] = 0xFF000000 | ((x ^ y) << 8);
				}
			}
		}

#ifdef _DEBUG
		std::cout << "Frame time: " << frametime << "ms\n";
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