#pragma once

#include <cstdint>
#include <chrono>
#include <iostream>
#include <algorithm>

#include "SDL3/SDL.h"

#include "state.h"
#include "helpers.h"
#include "geometry/drawing.h"
#include "geometry/geometry.h"

State state;
Frametimes frametimes;
bool running = true;

static void import_all_obj();
void debug_depth();
void handle_inputs();

int main(int argc, int8_t argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	state.window = SDL_CreateWindow("Software Renderer",
		WINDOW_WIDTH, WINDOW_HEIGHT, 0);

	state.renderer = SDL_CreateRenderer(state.window, nullptr);
	state.texture = SDL_CreateTexture(state.renderer,
		SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);

	double frametime = 0.0;

	import_all_obj();
	std::cout << "Importing all objs took " << state.last_frame_time << "ms\n";
	state.camera.pos.z = -10.0f;

	state.camera.compute_orientation();

	while (running) {
		while (SDL_PollEvent(&state.event)) {
			handle_inputs();
		}

		draw_frame();
		frametimes.add(state.last_frame_time);

#ifdef _DEBUG
		if (frametimes.frame_number % 20 == 0) {
			std::cout << "Frame time: " << frametimes.averageMs() << "ms\n";
			std::cout << "FPS: " << frametimes.averageFps() << "\n";
		}
#endif

		if (state.debug_depth) {
			debug_depth();
		}

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

static void import_all_obj() {
	Autotimer timer(&state.last_frame_time);
	state.drawables.push_back(import_from_obj("../goose.obj"));
}

void handle_inputs() {
	if (state.event.type == SDL_EVENT_QUIT) {
		running = false;
	}
	if (state.event.type == SDL_EVENT_KEY_DOWN) {
		const SDL_KeyboardEvent& ks = state.event.key;
		switch (ks.key) {
		case SDLK_F1:
			state.debug_depth = !state.debug_depth;
			state.debug_wireframe = false;
			break;
		case SDLK_F2:
			state.debug_wireframe = !state.debug_wireframe;
			state.debug_depth = false;
			break;
		case SDLK_W:
			state.camera.pos -= state.camera.forward.XYZ();
			state.camera.update_view_matrix();
			break;
		case SDLK_S:
			state.camera.pos += state.camera.forward.XYZ();
			state.camera.update_view_matrix();
			break;
		case SDLK_D:
			state.camera.yaw += DegToRad(15.0f);
			state.camera.compute_orientation();
			break;
		case SDLK_A:
			state.camera.yaw -= DegToRad(15.0f);
			state.camera.compute_orientation();
			break;
		}
	}
}

void debug_depth() {
	const int pixel_count = WINDOW_HEIGHT * WINDOW_WIDTH;
	for (int i = 0; i < pixel_count; i++) {
		float d = state.depth_buffer[i];
		
		d = std::clamp(d, 0.0f, 1.0f);

		// normalize to 0-255.
		uint8_t intensity = static_cast<uint8_t>((1.0f - d) * 255.0f);

		state.pixels[i] =
			(255u << 24) | // ARGB format
			(intensity << 16) |
			(intensity << 8) |
			(intensity);
	}
};
