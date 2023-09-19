#pragma once
#include "pch.h"

namespace Time {
	// Measures time period between succesive Tick() calls.
	class Timer {
	public:
		void Tick();
		float GetDeltaTime();
	private:
		std::chrono::steady_clock::time_point m_last_time = std::chrono::high_resolution_clock::now();
		float m_delta_time = 0.0f;
	};

	void Tick();
	float DeltaTime();
}