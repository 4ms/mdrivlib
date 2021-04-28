#pragma once
#include "drivers/stm32xx.h"
#include "irq_ctrl.h"

namespace mdrivlib
{
namespace stm32mp1
{
namespace core_a7
{
struct System {
	System() {
	}

	static void SetVectorTable(uint32_t reset_address) {
	}

	// ARM CA7 uses 5-bit priority: pri1 is upper 3 bits (0-7), pri2 is lower 2 bits (0-3)
	static void set_irq_priority(IRQn_Type irqn, uint32_t pri1, uint32_t pri2) {
		auto pri = ((pri1 & 0b111) << 5) | (pri2 & 0b11 << 3);
		GIC_SetPriority(irqn, pri);
	}

	static void disable_irq(IRQn_Type irqn) {
		IRQ_Disable(irqn);
	}

	static void enable_irq(IRQn_Type irqn) {
		auto status =
			IRQ_SetMode((IRQn_ID_t)irqn, IRQ_MODE_TRIG_LEVEL_HIGH | IRQ_MODE_CPU_0 /*ALL*/ | IRQ_MODE_TYPE_IRQ);
		IRQ_Enable(irqn);
	}
};
} // namespace core_a7
} // namespace stm32mp1
} // namespace mdrivlib
