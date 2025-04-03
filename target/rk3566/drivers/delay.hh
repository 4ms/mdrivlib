#pragma once
#include <cstdint>

namespace mdrivlib
{

// FIXME: Use timer
inline void HAL_Delay(uint32_t ms) {
	volatile int x = 0;
	for (unsigned i = 0; i < 200'000u * ms; i++) {
		x = x + 1;
	}
}
} // namespace mdrivlib
