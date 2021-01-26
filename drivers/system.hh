#pragma once
#include "rcc.hh"
#include "stm32xx.h"

using namespace mdrivlib;

// Todo: refactor for LL intead of HAL
struct System {
	System() {}

	static void SetVectorTable(uint32_t reset_address) { SCB->VTOR = reset_address & (uint32_t)0x1FFFFF80; }

	static void init_clocks(const RCC_OscInitTypeDef &osc_def,
							const RCC_ClkInitTypeDef &clk_def,
							const RCC_PeriphCLKInitTypeDef &pclk_def,
							const uint32_t systick_freq_hz = 1000) {
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

	static uint32_t encode_nvic_priority(uint32_t pri1, uint32_t pri2) {
		return NVIC_EncodePriority(NVIC_GetPriorityGrouping(), pri1, pri2);
	}

	static void enable_gpio_rcc(GPIO_TypeDef *port) {
		if (port == nullptr)
			return;
		RCCControl::GPIO::enable(reinterpret_cast<uint32_t>(port));
	}

	static unsigned adc_periph_to_num(ADC_TypeDef *ADCx) {
		if (ADCx == nullptr)
			return 0;
		else if (ADCx == ADC1)
			return 1;
		else if (ADCx == ADC2)
			return 2;
		else if (ADCx == ADC3)
			return 3;
		else
			return 0;
	}

	static void enable_adc_rcc(unsigned periph_num) {
		if (periph_num >= 1 && periph_num <= RCCControl::ADC::NumP)
			RCCControl::ADC::enable(periph_num);
	}

	static void enable_adc_rcc(ADC_TypeDef *ADCx) { RCCControl::ADC::enable(adc_periph_to_num(ADCx)); }

	static constexpr void enable_dma_rcc(const DMA_TypeDef *DMAx) {
		if (DMAx == nullptr)
			return;
		else if (DMAx == DMA1)
			RCCControl::DMA_1::enable();
		else if (DMAx == DMA2)
			RCCControl::DMA_2::enable();
	}

	static constexpr void enable_i2c_rcc(I2C_TypeDef *I2Cx) {
		if (I2Cx == nullptr)
			return;
		else if (I2Cx == I2C1)
			RCCControl::I2C_1::enable();
		else if (I2Cx == I2C2)
			RCCControl::I2C_2::enable();
		else if (I2Cx == I2C3)
			RCCControl::I2C_3::enable();
	}

	static constexpr void disable_i2c_rcc(I2C_TypeDef *I2Cx) {
		if (I2Cx == nullptr)
			return;
		else if (I2Cx == I2C1)
			RCCControl::I2C_1::disable();
		else if (I2Cx == I2C2)
			RCCControl::I2C_2::disable();
		else if (I2Cx == I2C3)
			RCCControl::I2C_3::disable();
	}

	static constexpr void enable_sai_rcc(SAI_TypeDef *SAIx) {
		if (SAIx == nullptr)
			return;
		else if (SAIx == SAI1)
			RCCControl::SAI_1::enable();
		else if (SAIx == SAI2)
			RCCControl::SAI_2::enable();
		else if (SAIx == SAI3)
			RCCControl::SAI_3::enable();
		else if (SAIx == SAI4)
			RCCControl::SAI_4::enable();
	}

	static constexpr void disable_sai_rcc(SAI_TypeDef *SAIx) {
		if (SAIx == nullptr)
			return;
		else if (SAIx == SAI1)
			RCCControl::SAI_1::disable();
		else if (SAIx == SAI2)
			RCCControl::SAI_2::disable();
		else if (SAIx == SAI3)
			RCCControl::SAI_3::disable();
		else if (SAIx == SAI4)
			RCCControl::SAI_4::disable();
	}

	static uint8_t tim_periph_to_num(TIM_TypeDef *TIMx) {
		if (TIMx == nullptr)
			return 0;
		else if (TIMx == TIM1)
			return 1;
		else if (TIMx == TIM2)
			return 2;
		else if (TIMx == TIM3)
			return 3;
		else if (TIMx == TIM4)
			return 4;
		else if (TIMx == TIM5)
			return 5;
		else if (TIMx == TIM6)
			return 6;
		else if (TIMx == TIM7)
			return 7;
		else if (TIMx == TIM8)
			return 8;
		else if (TIMx == TIM9)
			return 9;
		else if (TIMx == TIM10)
			return 10;
		else if (TIMx == TIM11)
			return 11;
		else if (TIMx == TIM12)
			return 12;
		else if (TIMx == TIM13)
			return 13;
		else if (TIMx == TIM14)
			return 14;
		else if (TIMx == TIM15)
			return 15;
		else if (TIMx == TIM16)
			return 16;
		else if (TIMx == TIM17)
			return 17;
		else
			return 0;
	}

	static void enable_tim_rcc(unsigned periph_num) {
		if (periph_num >= 1 && periph_num <= RCCControl::TIM::NumP)
			RCCControl::TIM::enable(periph_num);
	}
	static void enable_tim_rcc(TIM_TypeDef *TIMx) { enable_tim_rcc(tim_periph_to_num(TIMx)); }

	static IRQn_Type tim_periph_to_IRQn(TIM_TypeDef *TIM) {
		if (TIM == nullptr)
			return (IRQn_Type)(0);
		else if (TIM == TIM1)
			return TIM1_UP_TIM10_IRQn;
		else if (TIM == TIM2)
			return TIM2_IRQn;
		else if (TIM == TIM3)
			return TIM3_IRQn;
		else if (TIM == TIM4)
			return TIM4_IRQn;
		else if (TIM == TIM5)
			return TIM5_IRQn;
		else if (TIM == TIM6)
			return TIM6_DAC_IRQn;
		else if (TIM == TIM7)
			return TIM7_IRQn;
		else if (TIM == TIM8)
			return TIM8_UP_TIM13_IRQn;
		else if (TIM == TIM9)
			return TIM1_BRK_TIM9_IRQn;
		else if (TIM == TIM10)
			return TIM1_UP_TIM10_IRQn;
		else if (TIM == TIM11)
			return TIM1_TRG_COM_TIM11_IRQn;
		else if (TIM == TIM12)
			return TIM8_BRK_TIM12_IRQn;
		else if (TIM == TIM13)
			return TIM8_UP_TIM13_IRQn;
		else if (TIM == TIM14)
			return TIM8_TRG_COM_TIM14_IRQn;
		else if (TIM == TIM15)
			return TIM15_IRQn;
		else if (TIM == TIM16)
			return TIM16_IRQn;
		else if (TIM == TIM17)
			return TIM17_IRQn;
		else
			return (IRQn_Type)(0);
	}

	static uint32_t tim_periph_max_freq(TIM_TypeDef *TIM) {
		// Todo: get this to work on all arch
		// APB2 --> divider = 1;
		// APB1 --> divider = 2;
		uint32_t divider;
		auto TIMx_BASE_ADDR = reinterpret_cast<uintptr_t>(TIM);
		if (TIMx_BASE_ADDR >= APB1PERIPH_BASE && TIMx_BASE_ADDR < APB2PERIPH_BASE)
			divider = 2;
		else if (TIMx_BASE_ADDR >= APB2PERIPH_BASE)
			divider = 1;
		else
			divider = 1; // unknown, error?
		return HAL_RCC_GetHCLKFreq() / divider;
	}
};

