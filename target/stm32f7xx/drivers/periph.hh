#pragma once
#include "stm32xx.h"
#include <cstdint>

namespace mdrivlib::PeriphUtil
{
struct TIM {
	// NOTE: This is for STM32F746IEK
	constexpr static unsigned NumPeriph = 14;
	static uint8_t to_num(TIM_TypeDef *TIMx) {
		return TIMx == TIM1	 ? 1
			 : TIMx == TIM2	 ? 2
			 : TIMx == TIM3	 ? 3
			 : TIMx == TIM4	 ? 4
			 : TIMx == TIM5	 ? 5
			 : TIMx == TIM6	 ? 6
			 : TIMx == TIM7	 ? 7
			 : TIMx == TIM8	 ? 8
			 : TIMx == TIM9	 ? 9
			 : TIMx == TIM10 ? 10
			 : TIMx == TIM11 ? 11
			 : TIMx == TIM12 ? 12
			 : TIMx == TIM13 ? 13
			 : TIMx == TIM14 ? 14
							 : 0;
	}
	static IRQn_Type IRQn(TIM_TypeDef *TIMx) {
		return TIMx == TIM1	 ? TIM1_UP_TIM10_IRQn
			 : TIMx == TIM2	 ? TIM2_IRQn
			 : TIMx == TIM3	 ? TIM3_IRQn
			 : TIMx == TIM4	 ? TIM4_IRQn
			 : TIMx == TIM5	 ? TIM5_IRQn
			 : TIMx == TIM6	 ? TIM6_DAC_IRQn
			 : TIMx == TIM7	 ? TIM7_IRQn
			 : TIMx == TIM8	 ? TIM8_UP_TIM13_IRQn
			 : TIMx == TIM9	 ? TIM1_BRK_TIM9_IRQn
			 : TIMx == TIM10 ? TIM1_UP_TIM10_IRQn
			 : TIMx == TIM11 ? TIM1_TRG_COM_TIM11_IRQn
			 : TIMx == TIM12 ? TIM8_BRK_TIM12_IRQn
			 : TIMx == TIM13 ? TIM8_UP_TIM13_IRQn
			 : TIMx == TIM14 ? TIM8_TRG_COM_TIM14_IRQn
							 : (IRQn_Type)0;
	}
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
	static constexpr uint32_t max_period(unsigned tim_periph_num) {
		return 65535;
	}
	static constexpr uint32_t max_prescaler(unsigned tim_periph_num) {
		return 255;
	}
	static constexpr uint32_t max_clockdivider(unsigned tim_periph_num) {
		// if (tim_periph_num == 1)
		// 	return 255;
		return 255;
	}
	static constexpr uint32_t next_allowed_clockdivision(uint32_t tim_periph_num, uint32_t clock_division) {
		return clock_division + 1;
	}
};

} // namespace mdrivlib::PeriphUtil
