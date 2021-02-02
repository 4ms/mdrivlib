#pragma once
#include "adc_i2c_config_struct.hh"
#include "i2c.hh"
#include "stm32xx.h"

class ADC_Max11645 {
public:
	ADC_Max11645(I2CPeriph &i2c, const ADC_I2C_Config &conf)
		: _conf(conf) {
		//
	}

private:
	const ADC_I2C_Config &_conf;
};
