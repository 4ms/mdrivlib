#pragma once
#include "drivers/arch.hh"
#include "drivers/callable.hh"
#include "drivers/stm32xx.h"
#include <array>
#include <cstddef>

// FixMe: how to get around this ugly PP stuff?
#ifdef TESTPROJECT
#include "stubs/system.hh"
#define __BKPT()
#else
#include "drivers/interrupt_control.hh"
#endif

// template<typename T, size_t N>
// std::array<T, N> fill_arr(T val) {
// 	std::array<T, N> arr;
// 	for (auto &a : arr)
// 		a = val;
// 	return arr;
// }

namespace mdrivlib
{

template<typename T, size_t N>
auto fill_arr(auto &&f) {
	std::array<T, N> x;
	for (size_t i = 0l; i < N; i++)
		x[i] = f;
	return x;
}

// Interrupt Manager class
class Interrupt {
public:
	static constexpr uint32_t NumISRs = TargetName == Targets::stm32f0 ? 38
									  : TargetName == Targets::stm32f4 ? 85
									  : TargetName == Targets::stm32f7 ? 128
																	   : 256;
	using ISRType = Callback;
	using IRQType = IRQn_Type;

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
	// static void null_func() {
	// 	__BKPT();
	// }

	// static inline std::array<ISRType, NumISRs> ISRs{fill_arr<ISRType, NumISRs>(null_func)};
	static inline std::array<ISRType, NumISRs> ISRs;
};

using InterruptManager = Interrupt;

} // namespace mdrivlib
