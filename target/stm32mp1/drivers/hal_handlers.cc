#include "stm32xx.h"

extern "C" {
uint32_t HAL_GetTick(void) {
	return (PL1_GetCurrentPhysicalValue() / (HSE_VALUE / 2000)); // FIXME: Why is this off by 1?
}
}
