#pragma once
#include "pin.hh"
#include "spi_config_struct.hh"
#include "stm32xx.h"

struct DefaultSpiScreenConf {
	static constexpr unsigned ScreenSPIx = 0;
	static constexpr SpiConfig<ScreenSPIx> screen_SPI_conf{
		.IRQn = HardFault_IRQn,
		.priority1 = 3,
		.priority2 = 3,

		.SCLK = {},
		.COPI = {},
		.CIPO = {},
		.CS = {{}},

		.use_hardware_ss = true,
		.clock_division = 8,
		.data_size = 8,
		.data_dir = SpiDataDir::TXOnly,
	};
	using DCPin = FPin<GPIO::A, 0>;

	static constexpr uint32_t width = 240;
	static constexpr uint32_t height = 240;
};
