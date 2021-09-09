#pragma once
#include "drivers/callable.hh"
#include "stm32xx.h"
using IRQType = IRQn_Type;

// FixMe: how to get around this ugly PP stuff?
#ifdef TESTPROJECT
#include "stubs/system.hh"
#else
#include "interrupt_control.hh"
#endif

template<typename T, size_t N>
std::array<T, N> fill_arr(T val) {
	std::array<T, N> arr;
	for (auto &a : arr)
		a = val;
	return arr;
}

namespace mdrivlib
{

// Interrupt Manager class
class Interrupt {
public:
	static constexpr uint32_t NumISRs = 256;
	using ISRType = Callback;

	Interrupt() = default;
	Interrupt(IRQType irqnum, ISRType &&func) {
		ISRs[irqnum] = std::move(func);
	}

	// Register a callable object (e.g. lambda) to respond to an IRQ
	static void register_isr(IRQType irqnum, ISRType &&func) {
		ISRs[irqnum] = std::move(func);
	}

	// Register a callable object (e.g. lambda) to respond to an IRQ
	// Sets the priority and enables the IRQ immediately
	static void register_and_start_isr(IRQType irqnum, unsigned priority1, unsigned priority2, ISRType &&func) {
		InterruptControl::disable_irq(irqnum);
		InterruptControl::set_irq_priority(irqnum, priority1, priority2);
		ISRs[irqnum] = std::move(func);
		InterruptControl::enable_irq(irqnum);
	}

	static inline void callISR(uint32_t irqnum) {
		ISRs[irqnum]();
	}

private:
	static void null_func() {
		return;
	}

	static inline std::array<ISRType, NumISRs> ISRs{fill_arr<ISRType, NumISRs>(null_func)};
};

using InterruptManager = Interrupt;

} // namespace mdrivlib
