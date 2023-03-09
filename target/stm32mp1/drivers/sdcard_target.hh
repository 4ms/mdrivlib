#pragma once
#include "drivers/rcc.hh"
#include "drivers/stm32xx.h"

namespace mdrivlib
{
struct SDCardTarget {

	template<typename ConfT>
	constexpr static uint32_t get_base_clock_speed() {
		// FixMe: this should be done in Rcc Conf not here
		//RCC->SDMMC12CKSELR = RCC_SDMMC12CLKSOURCE_HSI; //3 = HSI = 64MHz
		constexpr uint32_t source_clock = 64'000'000;
		constexpr uint32_t base_clock = source_clock / 2;
		return base_clock;
	}
};
} // namespace mdrivlib
