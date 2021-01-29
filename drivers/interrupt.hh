#pragma once

#if defined(STM32F7) || defined(STM32H7) || defined(STM32F4) || defined(STM32MP1)
#include "stm32xx.h"
using IRQType = IRQn_Type;
#else
#include <cstdint>
using IRQType = uint32_t;
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

	// static inline void callISR(IRQType irqnum) { ISRs[irqnum](); }
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
