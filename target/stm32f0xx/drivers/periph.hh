#pragma once
#include "stm32xx.h"
#include <cstdint>

#ifdef ADC
#undef ADC
#endif

namespace mdrivlib::PeriphUtil
{

struct GPIO {
	// const static inline uint32_t NumPeriph = 3;
};
struct ADC {
	const static inline uint32_t NumPeriph = 1;
	static unsigned to_num(ADC_TypeDef *ADCx) {
		if (ADCx == ADC1)
			return 1;
		return 0;
	}
};
struct SAI {
	const static inline uint32_t NumPeriph = 0;
};
struct TIM {
	const static inline unsigned NumPeriph = 5;
	static uint8_t to_num(TIM_TypeDef *TIMx) {
		if (TIMx == nullptr)
			return 0;
		else if (TIMx == TIM1)
			return 1;
		else if (TIMx == TIM3)
			return 3;
		else if (TIMx == TIM14)
			return 14;
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
			return TIM1_BRK_UP_TRG_COM_IRQn;
		else if (TIM == TIM3)
			return TIM3_IRQn;
		else if (TIM == TIM14)
			return TIM14_IRQn;
		else if (TIM == TIM16)
			return TIM16_IRQn;
		else if (TIM == TIM17)
			return TIM17_IRQn;
		else
			return (IRQn_Type)(0);
	}
	static uint32_t max_freq(TIM_TypeDef *TIM) {
		return HAL_RCC_GetPCLK1Freq();
	}

	static constexpr uint32_t max_period(unsigned tim_periph_num) {
		return 0xFFFF;
	}
	static constexpr uint32_t max_prescaler(unsigned tim_periph_num) {
		return 0xFFFF;
	}
	static constexpr uint32_t max_clockdivider(unsigned tim_periph_num) {
		return 1; // can only be 1, 2, or 4.
	}
	static constexpr uint32_t next_allowed_clockdivision(uint32_t tim_periph_num, uint32_t clock_division) {
		if (clock_division == 1)
			return 2;
		if (clock_division == 2)
			return 4;
		if (clock_division == 4)
			return 4;
		return 1;
	}
};
} // namespace mdrivlib::PeriphUtil
