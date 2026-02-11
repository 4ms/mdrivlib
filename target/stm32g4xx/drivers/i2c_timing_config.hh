#pragma once
#include <bit>
#include <cstdint>

namespace mdrivlib
{

struct I2CTimingConfig {
	uint32_t SCLL : 8 = 0xB1;  // SCL low period
	uint32_t SCLH : 8 = 0x75;  // SCL high period
	uint32_t SDADEL : 4 = 0x0; // SDA Delay
	uint32_t SCLDEL : 4 = 0x7; // SCL Delay
	uint32_t : 4;
	uint32_t PRESC : 4 = 0x5; // Prescaler. (PRESC + 1) * tI2CCLK = tPRESC

	constexpr uint32_t calc() const {
#if defined(__cpp_lib_bit_cast)
		return std::bit_cast<uint32_t>(*this);
#else
		return *reinterpret_cast<uint32_t *>(this);
#endif
	}
};

} // namespace mdrivlib
