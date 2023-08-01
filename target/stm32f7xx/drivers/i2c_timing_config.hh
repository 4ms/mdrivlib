#pragma once
#include <cstdint>

namespace mdrivlib
{

struct I2CTimingConfig {
	uint8_t PRESC = 0x50;		  // top 4 bits: (PRESC + 1) * tI2CCLK = tPRESC
								  // bottom 4 bits is ignored
	uint8_t SCLDEL_SDADEL = 0x70; // top 4 bits: SCLDEL * tPRESC = SCL Delay between SDA edge and SCL
								  // rising edge bottom 4 bits: = SDADEL * tPRESC = SDA Delay between SCL
								  // falling edge and SDA edge
	uint8_t SCLH = 0x75;		  // SCL high period = (SCLH+1) * tPRESC
	uint8_t SCLL = 0xB1;		  // SCL low period = (SCLL+1) * tPRESC

	[[nodiscard]] constexpr uint32_t calc() const {
		return ((PRESC & 0xF0) << 24) | ((SCLDEL_SDADEL) << 16) | ((SCLH) << 8) | ((SCLL) << 0);
	}
};

// I2C clock = I2CPeriphClock / (SCLDEL + SDADEL + SCLH+1 + SCLL+1)
// I2CPeriphClock = ClockSource / (PRESC>>4 + 2)

// SCL Period = tSCL = tSYNC1 + tSYNC2 + ( (SCLH+1 + SCLL+1) * (PRESC>>4 + 1) * tClockSource) )
// tSYNC1 = (SDADEL * (PRESC>>4 + 1) + 1) * tClockSource
// tSYNC2 = (SCLDEL * (PRESC>>4 + 1) + 1) * tClockSource

// struct I2CTimingConfigBitfld {
// 	uint8_t PRESC : 4;	// 4 bits: (PRESC + 1) * tI2CCLK = tPRESC
// 	uint8_t SCLDEL : 4; // 4 bits: SCLDEL * tPRESC = SCL Delay between SDA edge and SCL rising edge
// 	uint8_t SDADEL : 4; // 4 bits: = SDADEL * tPRESC = SDA Delay between SCL falling edge and SDA edge
// 	uint8_t SCLH : 8;	// SCL high period = (SCLH+1) * tPRESC
// 	uint8_t SCLL : 8;	// SCL low period = (SCLL+1) * tPRESC

// 	constexpr operator uint32_t() {
// 		return (PRESC << 28) | (SCLDEL << 20) | (SDADEL << 16) | (SCLH << 8) | (SCLL << 0);
// 	}
// };

} // namespace mdrivlib
