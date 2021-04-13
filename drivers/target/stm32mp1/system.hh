#pragma once
#include "stm32xx.h"

// Todo: refactor to use CMSIS header intead of HAL (RCC_OscInitTypeDef, etc)
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
		auto pri = ((pri1 & 0b111) << 2) | (pri2 & 0b11);
		GIC_SetPriority(irqn, pri);
	}

	static void disable_irq(IRQn_Type irqn) {
		GIC_DisableIRQ(irqn);
	}

	static void enable_irq(IRQn_Type irqn) {
		GIC_EnableIRQ(irqn);
	}
};
} // namespace core_a7
} // namespace stm32mp1
} // namespace mdrivlib
