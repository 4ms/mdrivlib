#pragma once
#include "drivers/stm32xx.h"

namespace mdrivlib::PeriphUtil
{

struct GPIO {
	const static inline uint32_t NumPeriph = 12; // GPIOA-GPIOK + GPIOZ
};
struct ADC {
	const static inline uint32_t NumPeriph = 3;
	static unsigned to_num(ADC_TypeDef *ADCx) {
		if (ADCx == ADC1)
			return 1;
		if (ADCx == ADC2)
			return 2;
		if (ADCx == ADC3)
			return 3;
		return 0;
	}
};
struct SAI {
	const static inline uint32_t NumPeriph = 4;
};
struct TIM {
	const static inline unsigned NumPeriph = 20;
	static uint8_t to_num(TIM_TypeDef *TIMx) {
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
		else if (TIMx == TIM20)
			return 20;
		else
			return 0;
	}
	static IRQn_Type IRQn(TIM_TypeDef *TIM) {
		if (TIM == nullptr)
			return (IRQn_Type)(0);
		else if (TIM == TIM1)
			return TIM1_UP_IRQn;
		else if (TIM == TIM2)
			return TIM2_IRQn;
		else if (TIM == TIM3)
			return TIM3_IRQn;
		else if (TIM == TIM4)
			return TIM4_IRQn;
		else if (TIM == TIM5)
			return TIM5_IRQn;
		else if (TIM == TIM6)
			return TIM6_IRQn;
		else if (TIM == TIM7)
			return TIM7_IRQn;
		else if (TIM == TIM8)
			return TIM8_UP_IRQn;
		else if (TIM == TIM10)
			return TIM10_IRQn;
		else if (TIM == TIM11)
			return TIM11_IRQn;
		else if (TIM == TIM12)
			return TIM12_IRQn;
		else if (TIM == TIM13)
			return TIM13_IRQn;
		else if (TIM == TIM14)
			return TIM14_IRQn;
		else if (TIM == TIM15)
			return TIM15_IRQn;
		else if (TIM == TIM16)
			return TIM16_IRQn;
		else if (TIM == TIM17)
			return TIM17_IRQn;
		else if (TIM == TIM20)
			return TIM20_UP_IRQn;
		else
			return (IRQn_Type)(0);
	}
	static uint32_t max_freq(TIM_TypeDef *TIMx) {
		// clang-format off
		uint32_t timer_id = TIMx == TIM1  ? RCC_PERIPHCLK_TIM1
						  : TIMx == TIM2  ? RCC_PERIPHCLK_TIM2
						  : TIMx == TIM3  ? RCC_PERIPHCLK_TIM3
						  : TIMx == TIM4  ? RCC_PERIPHCLK_TIM4
						  : TIMx == TIM5  ? RCC_PERIPHCLK_TIM5
						  : TIMx == TIM6  ? RCC_PERIPHCLK_TIM6
						  : TIMx == TIM7  ? RCC_PERIPHCLK_TIM7
						  : TIMx == TIM8  ? RCC_PERIPHCLK_TIM8
						  : TIMx == TIM10 ? RCC_PERIPHCLK_TIM10
						  : TIMx == TIM11 ? RCC_PERIPHCLK_TIM11
						  : TIMx == TIM12 ? RCC_PERIPHCLK_TIM12
						  : TIMx == TIM13 ? RCC_PERIPHCLK_TIM13
						  : TIMx == TIM14 ? RCC_PERIPHCLK_TIM14
						  : TIMx == TIM15 ? RCC_PERIPHCLK_TIM15
						  : TIMx == TIM16 ? RCC_PERIPHCLK_TIM16
						  : TIMx == TIM17 ? RCC_PERIPHCLK_TIM17
						  : TIMx == TIM20 ? RCC_PERIPHCLK_TIM20
										  : 0xFFFFFFFF;
		// clang-format on
		if (timer_id == 0xFFFFFFFF)
			return 0;
		return HAL_RCCEx_GetTimerCLKFreq(timer_id);
	}

	static constexpr uint32_t max_period(unsigned tim_periph_num) {
		if (tim_periph_num == 2 || tim_periph_num == 5)
			return 0xFFFFFFFF;
		return 65535;
	}
	static constexpr uint32_t max_prescaler(unsigned tim_periph_num) {
		return 65535;
	}
	static constexpr uint32_t max_clockdivider(unsigned tim_periph_num) {
		return 1; // can only be 1, 2, or 4.
	}
	static constexpr uint32_t next_allowed_clockdivision(uint32_t tim_periph_num, uint32_t clock_division) {
		return clock_division + 1;
	}
};
} // namespace mdrivlib::PeriphUtil
