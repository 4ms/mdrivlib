#pragma once
#include "rcc.hh"
#include "stm32xx.h"

using namespace mdrivlib;

// Todo: refactor for LL intead of HAL
struct System {
	System() {}

	static void SetVectorTable(uint32_t reset_address);

	static void init_clocks(const RCC_OscInitTypeDef &osc_def,
							const RCC_ClkInitTypeDef &clk_def,
							const RCC_PeriphCLKInitTypeDef &pclk_def,
							const uint32_t systick_freq_hz = 1000);

	static uint32_t encode_nvic_priority(uint32_t pri1, uint32_t pri2);

	static void enable_gpio_rcc(GPIO_TypeDef *port);

	static unsigned adc_periph_to_num(ADC_TypeDef *ADCx);
	static void enable_adc_rcc(unsigned periph_num);
	static void enable_adc_rcc(ADC_TypeDef *ADCx);

	static constexpr void enable_dma_rcc(const DMA_TypeDef *DMAx);

	static constexpr void enable_i2c_rcc(I2C_TypeDef *I2Cx);
	static constexpr void disable_i2c_rcc(I2C_TypeDef *I2Cx);

	static constexpr void enable_sai_rcc(SAI_TypeDef *SAIx);
	static constexpr void disable_sai_rcc(SAI_TypeDef *SAIx);

	static uint8_t tim_periph_to_num(TIM_TypeDef *TIMx);
	static void enable_tim_rcc(unsigned periph_num);
	static void enable_tim_rcc(TIM_TypeDef *TIMx);
	static IRQn_Type tim_periph_to_IRQn(TIM_TypeDef *TIM);
	static uint32_t tim_periph_max_freq(TIM_TypeDef *TIM);
};

