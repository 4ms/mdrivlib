#pragma once
#include "stm32xx.h"

struct SPI_ADC_Config {
	SPI_TypeDef *spi;
	// pins:
	// with multiple CS pins (max 8)
	unsigned num_chips;
	unsigned chans_per_chip;
	uint32_t change_channel_commands[8];
	uint32_t continue_channel_commands[8];
};

struct AdcSpi_MAX11666 {
	AdcSpi_MAX11666(const SPI_ADC_Config conf) {
		// do stuff with conf...
	}
	uint16_t get_val(int chan) { return 0; }
};

