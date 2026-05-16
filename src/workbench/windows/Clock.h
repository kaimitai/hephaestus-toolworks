#ifndef HTW_CLOCK_H
#define HTW_CLOCK_H

#include <cstddef>
#include <numbers>

namespace htw {

	class Clock {
		std::uint64_t ticks;
		double seconds;
		double delta_seconds;
		std::uint64_t frame_no;

	public:
		Clock(void);
		void update();
		std::size_t cycle_index(std::size_t count, double seconds_per_frame = 0.25) const;
		double pulse(double period_seconds) const;
	};

}

#endif
