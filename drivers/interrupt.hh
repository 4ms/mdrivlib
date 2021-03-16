#pragma once

#include "stm32xx.h"
using IRQType = IRQn_Type;

// FixMe: how to get around this ugly PP stuff?
#ifdef TESTPROJECT
#include "stubs/system.hh"
#else
#include "drivers/system.hh"
#endif

#include <functional>

// Interrupt class
// register a callable object (e.g. lambda) to an IRQType
class Interrupt {
public:
	// Todo: Try inplace_function<void(void)> instead of std::function
	// https://github.com/WG21-SG14/SG14/blob/master/Docs/Proposals/NonAllocatingStandardFunction.pdf
	using ISRType = std::function<void(void)>;
	static inline const uint32_t NumISRs = 256;

	Interrupt() {
	}
	Interrupt(IRQType irqnum, ISRType &&func) {
		ISRs[irqnum] = std::move(func);
	}

	static void registerISR(IRQType irqnum, ISRType &&func) {
		ISRs[irqnum] = std::move(func);
	}

	static void registerISR(IRQType irqnum, unsigned priority1, unsigned priority2, ISRType &&func) {
		NVIC_DisableIRQ(irqnum);
		auto pri = System::encode_nvic_priority(priority1, priority2);
		NVIC_SetPriority(irqnum, pri);
		ISRs[irqnum] = std::move(func);
		NVIC_EnableIRQ(irqnum);
	}

	static inline void callISR(uint32_t irqnum) {
		ISRs[irqnum]();
	}

	// Sets a default handler for all ISRs.
	// This could be done for debug builds, to point to debug breakpoint
	// Copies the provided func, so a function pointer is recommended
	static inline void SetDefaultISR(ISRType func) {
		for (uint32_t i = 0; i < NumISRs; i++)
			ISRs[i] = func;
	}

private:
	static inline ISRType ISRs[NumISRs];
};

using InterruptManager = Interrupt;
