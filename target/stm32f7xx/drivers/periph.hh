#pragma once
#include "stm32xx.h"
#include <cstdint>

namespace mdrivlib::PeriphUtil
{
struct TIM {
	constexpr static unsigned NumPeriph = 14;
	constexpr uint8_t to_num(TIM_TypeDef *TIMx) {
		if (TIMx == nullptr)
			return 0;
		else if (TIMx == TIM1)
			return 1;
		else if (TIMx == TIM3)
			return 3;
		else if (TIMx == TIM14)
			return 14;
		else
			return 0;
	}
	static IRQn_Type IRQn(TIM_TypeDef *TIM) {
		if (TIM == nullptr)
			return (IRQn_Type)(0);
		else if (TIM == TIM1)
			return TIM1_CC_IRQn;
		else if (TIM == TIM3)
			return TIM3_IRQn;
		else
			return (IRQn_Type)(0);
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
