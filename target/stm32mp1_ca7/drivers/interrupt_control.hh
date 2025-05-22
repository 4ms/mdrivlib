#pragma once
#include "drivers/stm32xx.h"
#include "irq_ctrl.h"

namespace mdrivlib
{

struct InterruptControl {
	InterruptControl() = delete;

	// Binary Point is set to 4 in IRQ_Initialize().
	// From Table 3-2 in arm_gic_architecture_specifiation, the group and sub-group are:
	//     ggg.sssss
	// ARM CA7 uses 5-bit priority, and is left-aligned, so bottom 3 sss bits are ignored.
	// So we have:
	//     ggg.ssXXX
	//
	// So, pri1 is upper 3 bits (value can be 0-7), pri2 is next 2 bits (value can be 0-3), and lower 3 bits are ignored
	//
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
		uint32_t current_core = __get_MPIDR() & 0xFF; //0 = CA7 Core 1, 1 = CA7 Core 2
		GIC_SetTarget(irqn, 1 << current_core);		  //0 => 0b01, 1 => 0b10
		GIC_SetConfiguration(irqn, trig);
		GIC_ClearPendingIRQ(irqn);
		GIC_EnableIRQ(irqn);
	}

	static void reenable_irq(IRQn_Type irqn) {
		GIC_EnableIRQ(irqn);
	}
};
} // namespace mdrivlib
