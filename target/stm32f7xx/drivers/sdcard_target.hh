#pragma once
#include "drivers/rcc.hh"
#include "drivers/stm32xx.h"

namespace mdrivlib
{
struct SDCardTarget {

	template<typename ConfT>
	constexpr static uint32_t get_base_clock_speed() {
		constexpr uint32_t source_clock = 48'000'000;
		constexpr uint32_t base_clock = source_clock; // / 2;
		return base_clock;
	}
};
} // namespace mdrivlib
