#pragma once
#include "drivers/rcc.hh"
#include "stm32xx.h"

namespace mdrivlib::target
{

template<typename ConfT>
constexpr uint32_t setup_sdmmc_base_clk() {
	// No setup needed, just return base clock speed
	// This assumes rcc has been setup with 48MHz as the source clock
	constexpr uint32_t source_clock = 48'000'000;
	constexpr uint32_t base_clock = source_clock / 2;
	return base_clock;
}
} // namespace mdrivlib::target
