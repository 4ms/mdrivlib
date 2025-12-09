#pragma once
#include "drivers/hsem.hh"
#include "drivers/interrupt.hh"
#include "drivers/rcc.hh"
#include "drivers/stm32xx.h"

namespace mdrivlib
{
struct SystemClocks {
	SystemClocks() {
		NVIC_SetPriorityGrouping(0b101); // 2/2
		SystemCoreClock = HAL_RCC_GetSystemCoreClockFreq();
		SystemCoreClock /= 2;
		HAL_InitTick(TICK_INT_PRIORITY);
		SystemCoreClock *= 2;
	}
};
} // namespace mdrivlib
