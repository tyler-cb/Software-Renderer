#pragma once
#include <cstdint>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <numeric>

#define FRAMETIME_SAMPLES 20
#define PI 3.1415927f

// automatically times how long the timer stays in scope, and writes it to out.
struct Autotimer {
	using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;
	using clock = std::chrono::high_resolution_clock;
	using durationMs = std::chrono::duration<double, std::milli>;

	double* const output; // pointer itself is const
	const time_point start;

	explicit Autotimer(double* out) : output(out), start(clock::now()) {}

	~Autotimer() {
		time_point end = clock::now();
		durationMs elapsed = end - start;
		if (output) *output = elapsed.count();
//#ifdef _DEBUG
//		std::cout << "Timer completed: "<< std::fixed << std::setprecision(3) << elapsed.count() << " ms\n";
//#endif
	}
};

struct Frametimes {
	static constexpr uint8_t samples = FRAMETIME_SAMPLES;
	double times[samples]{};
	uint16_t frame_number = 0;

	void add(double frametime) {
		times[frame_number % samples] = frametime;
		frame_number++;
	}

	double averageMs() const {
		if (frame_number == 0) return 0.0;
		uint8_t max = (frame_number > samples) ? samples : frame_number;

		double sum = std::accumulate(times, times + max, 0.0);

		return sum / max;
	}

	double averageFps() const {
		double ms = averageMs();
		return (ms == 0.0) ? 0.0 : 1000.0 / ms;
	}
};

struct Colour {
	union {
		struct {
			uint8_t b, g, r, a;
		};
		uint32_t c;
	};

	Colour(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : b(b), g(g), r(r), a(a) {}
	Colour(uint8_t r, uint8_t g, uint8_t b) : b(b), g(g), r(r), a(255) {}
	Colour(uint32_t c) : c(c) {}
	Colour() : b(0), g(0), r(0), a(255) {}
};

constexpr float DegToRad(float degrees) { return degrees * PI / 180.0f; }
constexpr float RadToDeg(float radians) { return radians * 180.0f / PI; }