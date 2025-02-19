#pragma once
#include "drivers/aarch64_system_reg.hh"
#include "drivers/gic.hh"
#include <cstdint>

namespace mdrivlib
{

struct InterruptControl {
	InterruptControl() = delete;

	// Binary Point is set to 4 in IRQ_init().
	static void set_irq_priority(IRQn_Type irqn, uint32_t pri1, uint32_t pri2) {
		pri1 = pri1 > 7 ? 7 : pri1;
		pri2 = pri2 > 3 ? 3 : pri2;
		auto pri = (pri1 << 5) | (pri2 << 3);
		GIC_SetPriority(irqn, pri);
	}

	static void disable_irq(IRQn_Type irqn) {
		GIC_DisableIRQ(irqn);
	}

	enum TriggerType { LevelTriggered = 0b01, EdgeTriggered = 0b10 };
	static void enable_irq(IRQn_Type irqn, TriggerType trig = EdgeTriggered) {
		GIC_DisableIRQ(irqn);

		uint32_t current_core = get_core_id() & 0xFF; //0 = CA7 Core 1, 1 = CA7 Core 2
		GIC_SetTarget(irqn, current_core + 1);
		GIC_SetConfiguration(irqn, trig == LevelTriggered ? InterruptConfig::Level : InterruptConfig::Edge);
		GIC_ClearPendingIRQ(irqn);

		GIC_EnableIRQ(irqn);
	}
};
} // namespace mdrivlib
