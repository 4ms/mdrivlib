#pragma once
#include <cstdint>

namespace mdrivlib
{

using IRQn_Type = uint32_t;

struct InterruptControl {
	InterruptControl() = delete;

	static void set_irq_priority(IRQn_Type irqn, uint32_t pri1, uint32_t pri2) {
	}

	static void disable_irq(IRQn_Type irqn) {
	}

	static void enable_irq(IRQn_Type irqn) {
	}
};

} // namespace mdrivlib
