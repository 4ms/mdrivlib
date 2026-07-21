#pragma once
#include "drivers/stm32xx.h"
#include "drivers/system_reg.hh"

namespace mdrivlib
{

struct InterruptControl {
	InterruptControl() = delete;

	// Binary Point is set to 4 in IRQ_Initialize().
	// 4 -> Group priority: [7:5], Subpriority [4:0]
	// But only 5 bits are used, so we have:
	// gggSSxxx
	//
	static void set_irq_priority(IRQn_Type irqn, uint32_t pri1, uint32_t pri2) {
		pri1 = pri1 > 0b111 ? 0b111 : pri1;
		pri2 = pri2 > 0b11 ? 0b11 : pri2;
		auto pri = (pri1 << 5) | (pri2 << 3);
		GIC_SetPriority(irqn, pri);
	}

	static void disable_irq(IRQn_Type irqn) {
		GIC_DisableIRQ(irqn);
	}

	enum TriggerType { LevelTriggered = 0b01, EdgeTriggered = 0b10 };
	static void enable_irq(IRQn_Type irqn, TriggerType trig = EdgeTriggered) {
		GIC_DisableIRQ(irqn);

		uint32_t current_core = get_mpid() & 0xFF; // 0 = Core 1, 1 = Core 2
		GIC_SetTarget(irqn, (1 << current_core));
		GIC_SetConfiguration(irqn, trig == LevelTriggered ? 0b00 : 0b10);
		GIC_ClearPendingIRQ(irqn);

		GIC_EnableIRQ(irqn);
	}

	static void reenable_irq(IRQn_Type irqn) {
		GIC_EnableIRQ(irqn);
	}

	static bool is_enabled_irq(IRQn_Type irqn) {
		return GIC_GetEnableIRQ(irqn);
	}
};

} // namespace mdrivlib
