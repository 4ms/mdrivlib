#pragma once
#include "drivers/i2c_timing_config.hh"
#include "drivers/pin.hh"

namespace mdrivlib
{

struct I2CConfig {
	I2C_TypeDef *I2Cx;
	PinDef SCL;
	PinDef SDA;
	I2CTimingConfig timing;
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

} // namespace mdrivlib
