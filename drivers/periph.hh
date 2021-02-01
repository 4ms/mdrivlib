#pragma once
#include "stm32xx.h"
#include <cstdint>

namespace mdrivlib
{
namespace stm32h7x5xx
{
namespace peripherals
{
struct GPIO {
	const static inline uint32_t NumPeriph = 10;
};
struct ADC {
	const static inline uint32_t NumPeriph = 3;
	static unsigned to_num(ADC_TypeDef *ADCx) {
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
};
struct SAI {
	const static inline uint32_t NumPeriph = 4;
};
struct TIM {
	const static inline unsigned NumPeriph = 17;
	static uint8_t to_num(TIM_TypeDef *TIMx) {
		if (TIMx == nullptr)
			return 0;
		else if (TIMx == TIM1)
			return 1;
		else if (TIMx == TIM2)
			return 2;
		else if (TIMx == TIM3)
			return 3;
		else if (TIMx == TIM5)
			return 4;
		else if (TIMx == TIM5)
			return 5;
		else if (TIMx == TIM6)
			return 6;
		else if (TIMx == TIM7)
			return 7;
		else if (TIMx == TIM8)
			return 8;
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
	static IRQn_Type IRQn(TIM_TypeDef *TIM) {
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
	static uint32_t max_freq(TIM_TypeDef *TIM) {
		// Todo: get this to work on all arch
		// APB2 --> divider = 1;
		// APB1 --> divider = 2;
		uint32_t divider;
		auto TIMx_BASE_ADDR = reinterpret_cast<uintptr_t>(TIM);

		// Todo: determine bus of this TIM (PB1, HB4, etc.)
		// Then determine divider of that bus, based on RCC settings

		divider = 2;
		return HAL_RCC_GetHCLKFreq() / divider;
	}
};
} // namespace peripherals
} // namespace stm32h7x5xx

namespace stm32f7xx
{
namespace peripherals
{
struct TIM {
	static uint32_t max_freq(TIM_TypeDef *TIM) {
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

} // namespace peripherals

} // namespace stm32f7xx

} // namespace mdrivlib

