#pragma once
#include "drivers/rcc.hh"
#include "stm32xx.h"

namespace mdrivlib::target
{

template<typename ConfT>
constexpr uint32_t setup_sdmmc_base_clk() {
	RCC->SDMMC12CKSELR = RCC_SDMMC12CLKSOURCE_HSI; //3 = HSI = 64MHz
	constexpr uint32_t base_clock = 64'000'000;
	return base_clock / 2;
}
} // namespace mdrivlib::target
