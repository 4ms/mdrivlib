#pragma once
#include "pin.hh"

namespace mdrivlib
{

struct ADC_I2C_Config {
	//
};

struct MuxedADC_Config {
	PinDef SEL0;
	PinDef SEL1;
	PinDef SEL2;
	ADC_I2C_Config adc_conf;
};

struct DualMuxedADC_Config {
	PinDef SEL0;
	PinDef SEL1;
	PinDef SEL2;
	PinDef bSEL0;
	PinDef bSEL1;
	PinDef bSEL2;
	ADC_I2C_Config adc_conf;
};
} // namespace mdrivlib
