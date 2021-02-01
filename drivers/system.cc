#include "system.hh"
#include "arch.hh"
#include "periph.hh"
#include "rcc.hh"

void System::SetVectorTable(uint32_t reset_address) {
	SCB->VTOR = reset_address & (uint32_t)0x1FFFFF80;
}

void System::init_clocks(const RCC_OscInitTypeDef &osc_def,
						 const RCC_ClkInitTypeDef &clk_def,
						 const RCC_PeriphCLKInitTypeDef &pclk_def,
						 const uint32_t systick_freq_hz) {
#if defined(STM32H745xx) || defined(STM32H755xx)
	HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);
	while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
	}
	__HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
#endif

#if defined(STM32F7) || defined(STM32F4)
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
#endif

#ifndef STM32MP1
	// Todo: const_cast or fix HAL_RCC_ functions to take a const struct
	RCC_OscInitTypeDef osc_def_ = osc_def;
	HAL_RCC_OscConfig(&osc_def_);

#ifdef STM32F7
	HAL_PWREx_EnableOverDrive();
#endif

	RCC_ClkInitTypeDef clk_def_ = clk_def;
	HAL_RCC_ClockConfig(&clk_def_, FLASH_LATENCY_7);

	RCC_PeriphCLKInitTypeDef pclk_def_ = pclk_def;
	HAL_RCCEx_PeriphCLKConfig(&pclk_def_);

	HAL_RCC_EnableCSS();

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / systick_freq_hz);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
#endif
}

uint32_t System::encode_nvic_priority(uint32_t pri1, uint32_t pri2) {
	return NVIC_EncodePriority(NVIC_GetPriorityGrouping(), pri1, pri2);
}

