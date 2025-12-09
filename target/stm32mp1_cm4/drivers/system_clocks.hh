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
		HAL_InitTick(TICK_INT_PRIORITY);
	}
};
} // namespace mdrivlib
