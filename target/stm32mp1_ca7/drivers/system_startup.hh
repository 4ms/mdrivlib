#pragma once
#include "drivers/stm32xx.h"

namespace mdrivlib
{

struct SystemStartup {
	//FIXME: osc_def is not used
	static void init_clocks(const RCC_OscInitTypeDef &osc_def,
							const RCC_ClkInitTypeDef &clk_def,
							const RCC_PeriphCLKInitTypeDef &pclk_def,
							const uint32_t systick_freq_hz = 1000) {
		RCC_ClkInitTypeDef clk_def_ = clk_def;
		if (HAL_RCC_ClockConfig(&clk_def_) != HAL_OK)
			__BKPT();

		RCC_PeriphCLKInitTypeDef pclk_def_ = pclk_def;
		if (HAL_RCCEx_PeriphCLKConfig(&pclk_def_) != HAL_OK)
			__BKPT();
	}
};
} // namespace mdrivlib
