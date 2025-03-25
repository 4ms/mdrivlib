#pragma once
#include <cstdint>

namespace RockchipPeriph
{

inline uint32_t masked_set_bit(uint8_t bit) {
	return (1 << bit) | (1 << (bit + 16));
}

inline uint32_t masked_clr_bit(uint8_t bit) {
	return (0 << bit) | (1 << (bit + 16));
}

inline uint32_t masked(uint16_t mask, uint16_t bits) {
	return (mask << 16) | bits;
}

} // namespace RockchipPeriph
