#pragma once
#include "drivers/stm32xx.h"

// NVIC control for the Cortex-M33. Same API as target/stm32mp1_cm4 and stm32mp2_ca35.

namespace mdrivlib
{
struct InterruptControl {
	InterruptControl() = delete;

	static void SetVectorTable(uint32_t reset_address) {
		SCB->VTOR = reset_address;
	}

	static void set_irq_priority(IRQn_Type irqn, uint32_t pri1, uint32_t pri2) {
		auto pri = NVIC_EncodePriority(NVIC_GetPriorityGrouping(), pri1, pri2);
		NVIC_SetPriority(irqn, pri);
	}

	static void disable_irq(IRQn_Type irqn) {
		NVIC_DisableIRQ(irqn);
	}

	// TriggerType is here for CA35 <=> CM33 API compatibility
	enum TriggerType { LevelTriggered = 0b01, EdgeTriggered = 0b10 };
	static void enable_irq(IRQn_Type irqn, TriggerType = EdgeTriggered) {
		NVIC_EnableIRQ(irqn);
	}

	static void reenable_irq(IRQn_Type irqn) {
		NVIC_EnableIRQ(irqn);
	}
};
} // namespace mdrivlib
