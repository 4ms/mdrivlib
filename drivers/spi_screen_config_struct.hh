#pragma once
#include "interrupt.hh"
#include "pin.hh"
#include "spi_config_struct.hh"
#include "stm32xx.h"

struct DefaultSpiScreenConf {
	struct ScreenSpiConf : DefaultSpiConf {
		static constexpr uint16_t PeriphNum = 1;
		static constexpr uint16_t NumChips = 1;
		static constexpr IRQType IRQn = SPI1_IRQn;
		static constexpr uint16_t priority1 = 3;
		static constexpr uint16_t priority2 = 3;
		static constexpr PinNoInit SCLK = {GPIO::A, 0, 0};
		static constexpr PinNoInit COPI = {GPIO::A, 0, 0};
		static constexpr PinNoInit CIPO = {GPIO::A, 0};
		static constexpr PinNoInit CS0 = {GPIO::A, 0, 0};
		static constexpr PinNoInit CS1 = {GPIO::A, 0, 0};
		static constexpr PinNoInit CS2 = {GPIO::A, 0, 0};
		static constexpr PinNoInit CS3 = {GPIO::A, 0, 0};
		static constexpr bool use_hardware_ss = true;
		static constexpr uint16_t clock_division = 8;
		static constexpr uint16_t data_size = 8;
		static constexpr SpiDataDir data_dir = SpiDataDir::TXOnly;
	};
	using DCPin = FPin<GPIO::A, 0>;

	static constexpr uint32_t width = 240;
	static constexpr uint32_t height = 240;
};
