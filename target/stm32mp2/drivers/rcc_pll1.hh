#pragma once
#include "drivers/stm32xx.h"
#include <cstdint>

namespace mdrivlib
{

// Sets the main CPU1 PLL1 to HSE * multiplier
// Typically HSE is 40MHz
inline void set_pll1(uint16_t multiplier) {
	// Use ext2f for A35 clock and wait until clock switches
	CA35SSC->CHGCLKREQ_RW = 0x00010001;
	while ((CA35SSC->CHGCLKREQ_RW & 0b10) == 0b00)
		;

	// Set PLL1, and wait until locked
	CA35SSC->PLL_FREQ1_RW = 0x00010000 | multiplier;
	CA35SSC->PLL_FREQ2_RW = 0x00000009; // div 1, div 1
	while ((CA35SSC->PLL_ENABLE_RW & 0b10) == 0b00)
		;

	// Use PLL1 for A35 clock
	CA35SSC->CHGCLKREQ_RW = 0x00010000;
	while ((CA35SSC->CHGCLKREQ_RW & 0b10) == 0b10)
		;
}

} // namespace mdrivlib
