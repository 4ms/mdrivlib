#pragma once
#include "drivers/rcc.hh"
#include "stm32xx.h"

namespace mdrivlib
{

template<typename ConfT>
struct SDMMCTarget {

	static void setup_base_clk() {
		RCC->SDMMC12CKSELR = RCC_SDMMC12CLKSOURCE_HSI; //3 = HSI = 64MHz
	}

	static uint32_t get_base_clk() {
		constexpr uint32_t base_clock = 64'000'000;
		return base_clock / 2;
	}
};

} // namespace mdrivlib
