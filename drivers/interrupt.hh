#pragma once

#include "stm32xx.h"
using IRQType = IRQn_Type;

// FixMe: how to get around this ugly PP stuff?
#ifdef TESTPROJECT
#include "stubs/system.hh"
#else
#include "interrupt_control.hh"
#endif

#include "drivers/callable.hh"

namespace mdrivlib
{

// Interrupt Manager class
class Interrupt {
public:
	using ISRType = Callback;

	static inline const uint32_t NumISRs = 256;

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

	// Sets a default handler for all ISRs.
	// This could be done for debug builds, to point to debug breakpoint
	// Note: this copies the provided func
	static inline void SetDefaultISR(ISRType func) {
		for (auto &ISR : ISRs)
			ISR = func;
	}

private:
	static inline ISRType ISRs[NumISRs];
};

using InterruptManager = Interrupt;

} // namespace mdrivlib

// Todo:
// 		- Test SetDefaultISR()
// 		- Try alternatives to std::function
// 			* inplace_function<void(void)>
//			  https://github.com/WG21-SG14/SG14/blob/master/Docs/Proposals/NonAllocatingStandardFunction.pdf
//			* function_view
//			  https://vittorioromeo.info/index/blog/passing_functions_to_functions.html#fn_view_impl
//
//
