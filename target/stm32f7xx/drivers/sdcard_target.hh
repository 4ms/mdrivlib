#pragma once
#include "drivers/rcc.hh"
#include "stm32xx.h"

namespace mdrivlib
{

template<typename ConfT>
struct SDMMCTarget {

	static void setup_base_clk() {
		//TODO: ensure 48MHz clock source?
	}

	static uint32_t get_base_clk() {
		// This assumes rcc has been setup with 48MHz as the source clock
		constexpr uint32_t source_clock = 48'000'000;
		constexpr uint32_t base_clock = source_clock / 2;
		return base_clock;
	}
};

} // namespace mdrivlib
