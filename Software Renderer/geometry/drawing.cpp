#include "../state.h"
#include "../helpers.h"
#include "geometry.h"

void draw_frame() {
	autotimer timer(&state.last_frame_time);
#pragma omp parallel for
	for (int y = 0; y < WINDOW_HEIGHT; y++) {
		int row_offset = y * WINDOW_WIDTH;
		for (int x = 0; x < WINDOW_WIDTH; x++) {
			state.pixels[row_offset + x] = 0xFF000000 | ((x ^ y) << 8);
		}
	}
}

