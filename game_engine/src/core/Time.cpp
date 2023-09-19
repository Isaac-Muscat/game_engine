#include "pch.h"
#include "Time.h"

static Time::Timer s_timer;

void Time::Tick() {
	s_timer.Tick();
}

float Time::DeltaTime() {
	return s_timer.GetDeltaTime();
}

void Time::Timer::Tick() {
	auto current_time = std::chrono::high_resolution_clock::now();
	m_delta_time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - m_last_time).count();
	m_last_time = current_time;
}

float Time::Timer::GetDeltaTime() {
	return m_delta_time;
}
