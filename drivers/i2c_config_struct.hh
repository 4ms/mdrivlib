#pragma once
#include "pin.hh"

namespace mdrivlib
{
//FIXME: this needs to be device-specific or else more generic
// The layout below is for F7 and H7 and MP1, but not F446 for example
struct I2CTimingConfig {
	uint8_t PRESC;		   // top 4 bits: (PRESC + 1) * tI2CCLK = tPRESC
						   // bottom 4 bits is ignored
	uint8_t SCLDEL_SDADEL; // top 4 bits: SCLDEL * tPRESC = SCL Delay between SDA edge and SCL
						   // rising edge bottom 4 bits: = SDADEL * tPRESC = SDA Delay between SCL
						   // falling edge and SDA edge
	uint8_t SCLH;		   // SCL high period = (SCLH+1) * tPRESC
	uint8_t SCLL;		   // SCL low period = (SCLL+1) * tPRESC

	[[nodiscard]] constexpr uint32_t calc() const {
		return ((PRESC & 0xF0) << 24) | ((SCLDEL_SDADEL) << 16) | ((SCLH) << 8) | ((SCLL) << 0);
	}
};

struct I2CTimingConfigBitfld {
	uint8_t PRESC : 4;	// 4 bits: (PRESC + 1) * tI2CCLK = tPRESC
	uint8_t SCLDEL : 4; // 4 bits: SCLDEL * tPRESC = SCL Delay between SDA edge and SCL rising edge
	uint8_t SDADEL : 4; // 4 bits: = SDADEL * tPRESC = SDA Delay between SCL falling edge and SDA edge
	uint8_t SCLH : 8;	// SCL high period = (SCLH+1) * tPRESC
	uint8_t SCLL : 8;	// SCL low period = (SCLL+1) * tPRESC

	constexpr operator uint32_t() {
		return (PRESC << 28) | (SCLDEL << 20) | (SDADEL << 16) | (SCLH << 8) | (SCLL << 0);
	}
};

struct I2CConfig {
	I2C_TypeDef *I2Cx;
	PinNoInit SCL;
	PinNoInit SDA;
	I2CTimingConfig timing;
	//TODO:
	//I2CTimingConfigBitfld timing;
	unsigned priority1;
	unsigned priority2;

	bool analog_filter = true;
	enum DigitalFilterLevel {
		Min = 0,
		_1 = 1,
		_2 = 2,
		_3 = 3,
		_4 = 4,
		_5 = 5,
		_6 = 6,
		_7 = 7,
		_8 = 8,
		_9 = 9,
		_10 = 10,
		_11 = 11,
		_12 = 12,
		_13 = 13,
		_14 = 14,
		Max = 15,
	} digital_filter = _8;
};
//
// I2C clock = I2CPeriphClock / (SCLDEL + SDADEL + SCLH+1 + SCLL+1)
// I2CPeriphClock = ClockSource / (PRESC>>4 + 2)

// SCL Period = tSCL = tSYNC1 + tSYNC2 + ( (SCLH+1 + SCLL+1) * (PRESC>>4 + 1) * tClockSource) )
// tSYNC1 = (SDADEL * (PRESC>>4 + 1) + 1) * tClockSource
// tSYNC2 = (SCLDEL * (PRESC>>4 + 1) + 1) * tClockSource

} // namespace mdrivlib
