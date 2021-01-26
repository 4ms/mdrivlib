#pragma once

#include "stm32xx.h"

#if defined(STM32F7) || defined(STM32H7) || defined(STM32F4) || defined(STM32MP1)
using IRQType = IRQn_Type;
#else
#include <cstdint>
using IRQType = uint32_t;
#endif

#include <functional>

class Interrupt {
public:
	// Todo: Try inplace_function<void(void)> instead of std::function
	// https://github.com/WG21-SG14/SG14/blob/master/Docs/Proposals/NonAllocatingStandardFunction.pdf
	using ISRType = std::function<void(void)>;

	Interrupt() {}
	static void registerISR(IRQType irqnum, ISRType &&func) { ISRs[irqnum] = std::move(func); }
	Interrupt(IRQType irqnum, ISRType &&func) { ISRs[irqnum] = std::move(func); }
	static inline void callISR(IRQType irqnum) { ISRs[irqnum](); }
	static inline void callISR(unsigned irqnum) { ISRs[irqnum](); }
	static inline void Default_IRQHandler_() { __BKPT(); }

private:
	static inline ISRType ISRs[256] = {Default_IRQHandler_};
};

using InterruptManager = Interrupt;
