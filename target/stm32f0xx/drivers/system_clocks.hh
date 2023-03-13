#pragma once
#include "drivers/rcc.hh"
#include "drivers/stm32xx.h"

namespace mdrivlib
{
struct SystemClocks {
	SystemClocks() = delete;

	static void init_clocks(const RCC_OscInitTypeDef &osc_def,
							const RCC_ClkInitTypeDef &clk_def,
							const RCC_PeriphCLKInitTypeDef &pclk_def,
							const uint32_t systick_freq_hz = 1000) {

		// Todo: const_cast or fix HAL_RCC_ functions to take a const struct
		RCC_OscInitTypeDef osc_def_ = osc_def;
		HAL_RCC_OscConfig(&osc_def_);

		RCC_ClkInitTypeDef clk_def_ = clk_def;
		HAL_RCC_ClockConfig(&clk_def_, FLASH_LATENCY_1);

		// RCC_PeriphCLKInitTypeDef pclk_def_ = pclk_def;
		// HAL_RCCEx_PeriphCLKConfig(&pclk_def_);

		FLASH->ACR = FLASH->ACR | FLASH_ACR_PRFTBE;

		HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / systick_freq_hz);
		HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

		poweron();
	}
};

} // namespace mdrivlib
