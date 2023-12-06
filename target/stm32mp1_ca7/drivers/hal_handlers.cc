#include "drivers/stm32xx.h"

// Note: STGEN source must be set to HSE (which is 24MHz) in order for this to be accurate
inline constexpr float TickConversion = (float)(24000000UL >> 6) / 1000.f;

extern "C" {
uint32_t HAL_GetTick(void) {
	// This left shifts, ORs, uses FPU to convert to float, then does float multiplication and uses FPU to convert back
	uint32_t ticks_div_64 = PL1_GetCurrentPhysicalValue() >> 6;
	return (float)ticks_div_64 / TickConversion;

	// This uses a library function to convert u64 to float, then does float division
	// return (uint32_t)((float)PL1_GetCurrentPhysicalValue() / TicksPerMs);

	// This does integer division with a library function, very slow
	// return (PL1_GetCurrentPhysicalValue() / (HSE_VALUE / 1000));
}
}
