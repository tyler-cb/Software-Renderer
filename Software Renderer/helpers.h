#pragma once
#include <cstdint>
#include <chrono>
#include <iostream>
#include <iomanip>

// automatically times how long the timer stays in scope, and writes it to out.
struct autotimer {
	using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;
	using clock = std::chrono::high_resolution_clock;
	using durationMs = std::chrono::duration<double, std::milli>;

	double* const output; // pointer itself is const
	const time_point start;

	autotimer(double* out) : output(out), start(std::chrono::high_resolution_clock::now()) {}

	~autotimer() {
		time_point end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = end - start;
		if (output) *output = elapsed.count();
//#ifdef _DEBUG
//		std::cout << "Timer completed: "<< std::fixed << std::setprecision(3) << elapsed.count() << " ms\n";
//#endif
	}
};