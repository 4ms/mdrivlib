#pragma once
#include "pin.hh"
#include "spi_config_struct.hh"
#include "stm32xx.h"

struct DefaultSpiTransferConf {
	struct SpiConf : DefaultSpiConf {
		static constexpr uint16_t NumChips = 1;
		static constexpr bool use_hardware_ss = true;
		static constexpr SpiDataDir data_dir = SpiDataDir::TXOnly;
	};
	using AuxPin = FPin<GPIO::A, 0>;
};

