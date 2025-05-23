#pragma once
#include "drivers/stm32xx.h"

namespace mdrivlib
{

struct InterruptControl {
	InterruptControl() = delete;

	static void set_irq_priority(IRQn_Type irqn, uint32_t pri1, uint32_t pri2) {
		auto pri = NVIC_EncodePriority(NVIC_GetPriorityGrouping(), pri1, pri2);
		NVIC_SetPriority(irqn, pri);
	}

	static void disable_irq(IRQn_Type irqn) {
		NVIC_DisableIRQ(irqn);
	}

	static void enable_irq(IRQn_Type irqn) {
		NVIC_EnableIRQ(irqn);
	}

	static void reenable_irq(IRQn_Type irqn) {
		NVIC_EnableIRQ(irqn);
	}
};

} // namespace mdrivlib
