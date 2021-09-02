#pragma once
#include "pin.hh"

namespace mdrivlib
{

struct ADC_I2C_Config {
	//
};

struct MuxedADC_Config {
	PinNoInit SEL0;
	PinNoInit SEL1;
	PinNoInit SEL2;
	ADC_I2C_Config adc_conf;
};

struct DualMuxedADC_Config {
	PinNoInit SEL0;
	PinNoInit SEL1;
	PinNoInit SEL2;
	PinNoInit bSEL0;
	PinNoInit bSEL1;
	PinNoInit bSEL2;
	ADC_I2C_Config adc_conf;
};
} // namespace mdrivlib
