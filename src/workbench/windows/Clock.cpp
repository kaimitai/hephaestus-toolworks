#include "Clock.h"
#include <SDL3/SDL.h>
#include <cmath>

htw::Clock::Clock(void) :
	ticks{ 0 }, seconds{ 0 }, delta_seconds{ 0 }, frame_no{ 0 }
{
}

void htw::Clock::update() {
	const auto now{ SDL_GetTicksNS() };

	if (ticks == 0) {
		ticks = now;
		return;
	}

	const auto prev{ ticks };
	ticks = now;

	delta_seconds = static_cast<double>(ticks - prev) / 1'000'000'000.0;
	seconds += delta_seconds;
	++frame_no;
}

std::size_t htw::Clock::cycle_index(std::size_t count, double seconds_per_frame) const {
	if (count == 0 || seconds_per_frame <= 0.0)
		return 0;

	return static_cast<std::size_t>(seconds / seconds_per_frame) % count;
}

double htw::Clock::pulse(double period_seconds) const {
	if (period_seconds <= 0.0)
		return 0.0;

	const auto phase{ std::fmod(seconds, period_seconds) / period_seconds };
	return 0.5 + 0.5 * std::sin(phase * 2.0 * std::numbers::pi);
}
