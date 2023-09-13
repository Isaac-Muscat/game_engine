#include "pch.h"
#include "Time.h"

static float delta_time = 0;

void Time::Tick() {
	static auto start_time = std::chrono::high_resolution_clock::now();
	auto current_time = std::chrono::high_resolution_clock::now();
	delta_time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();
}

float Time::DeltaTime() {
	return delta_time;
}
