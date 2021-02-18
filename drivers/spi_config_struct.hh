#pragma once
#include "interrupt.hh"
#include "pin.hh"

enum SpiDataDir { Duplex, TXOnly, RXOnly, HalfDuplex };

struct DefaultSpiConf {
	static constexpr uint16_t PeriphNum = 1; // SPI1
	static constexpr uint16_t NumChips = 1;
	static constexpr IRQType IRQn = SPI1_IRQn;
	static constexpr uint16_t priority1 = 3;
	static constexpr uint16_t priority2 = 3;
	static constexpr PinNoInit SCLK = {GPIO::A, 0, LL_GPIO_AF_0};
	static constexpr PinNoInit COPI = {GPIO::A, 0, LL_GPIO_AF_0};
	static constexpr PinNoInit CIPO = {GPIO::A, 0, LL_GPIO_AF_0};
	static constexpr PinNoInit CS0 = {GPIO::A, 0, LL_GPIO_AF_0};
	static constexpr PinNoInit CS1 = {GPIO::A, 0, LL_GPIO_AF_0};
	static constexpr PinNoInit CS2 = {GPIO::A, 0, LL_GPIO_AF_0};
	static constexpr PinNoInit CS3 = {GPIO::A, 0, LL_GPIO_AF_0};
	static constexpr bool use_hardware_ss = false;
	static constexpr uint16_t clock_division = 64;
	static constexpr uint16_t data_size = 16;
	static constexpr SpiDataDir data_dir = SpiDataDir::Duplex;

	static constexpr uint32_t NumClocksToggleSSInterData = 0;

	static constexpr uint8_t FifoThreshold = 1;
};

