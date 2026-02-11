#pragma once
#include "drivers/stm32xx.h"
#include <cstdint>

namespace mdrivlib::PeriphUtil
{

struct TIM {
	constexpr static unsigned NumPeriph = 11;
	static uint8_t to_num(TIM_TypeDef *TIMx) {
		return TIMx == TIM1 ? 1
			 : TIMx == TIM2 ? 2
			 : TIMx == TIM3 ? 3
			 : TIMx == TIM4 ? 4
			 : TIMx == TIM5 ? 5
			 : TIMx == TIM6 ? 6
			 : TIMx == TIM7 ? 7
			 : TIMx == TIM8 ? 8
							: 0;
	}
	static IRQn_Type IRQn(TIM_TypeDef *TIMx) {
		return TIMx == TIM1 ? TIM1_UP_TIM16_IRQn
			 : TIMx == TIM2 ? TIM2_IRQn
			 : TIMx == TIM3 ? TIM3_IRQn
			 : TIMx == TIM4 ? TIM4_IRQn
			 : TIMx == TIM5 ? TIM1_BRK_TIM15_IRQn
			 : TIMx == TIM6 ? TIM6_DAC_IRQn
			 : TIMx == TIM7 ? TIM7_IRQn
			 : TIMx == TIM8 ? TIM8_UP_IRQn
							: (IRQn_Type)0;
	}

	//TODO: check if these are correct:
	static uint32_t max_freq(TIM_TypeDef *TIM) {
		// TODO:
		return HAL_RCC_GetHCLKFreq();
	}
	static constexpr uint32_t max_period(unsigned tim_periph_num) {
		return 65535;
	}
	static constexpr uint32_t max_prescaler(unsigned tim_periph_num) {
		return 255;
	}
	static constexpr uint32_t max_clockdivider(unsigned tim_periph_num) {
		return 255;
	}
	static constexpr uint32_t next_allowed_clockdivision(uint32_t tim_periph_num, uint32_t clock_division) {
		return clock_division + 1;
	}
};

} // namespace mdrivlib::PeriphUtil
