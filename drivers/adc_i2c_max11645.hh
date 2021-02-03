#pragma once
#include "adc_i2c_config_struct.hh"
#include "i2c.hh"
#include "pin.hh"
#include "stm32xx.h"

// tACQ >= 95 * (Rsource + Rin) * Cin
// 58 uS >= 95 * (25k + 2.5k) * 22pF, where 25k = max source impedance (100k pot with wiper in center)
// tACQ takes 12 clock cycles ==> 206kHz fSCL

// To enter HS mode (>400kHz), master must send 0000 1XXX  on bus to all devices

class ADC_Max11645 {
public:
	ADC_Max11645(I2CPeriph &i2c, const ADC_I2C_Config &conf)
		: _conf(conf) {
		//
	}

	void send_config();
	void start_acquisition(int chan) {
	}

private:
	const ADC_I2C_Config &_conf;
};

