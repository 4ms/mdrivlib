#pragma once
#include "interrupt.hh"
#include "pin.hh"

enum SpiDataDir { Duplex, TXOnly, RXOnly, HalfDuplex };

template<unsigned PERIPH_NUM, unsigned NUM_CHIPS = 1>
struct SpiConfig {
	static inline const unsigned PeriphNum = PERIPH_NUM;
	static inline const unsigned NumChips = NUM_CHIPS;

	IRQType IRQn;
	uint8_t priority1;
	uint8_t priority2;

	PinNoInit SCLK;
	PinNoInit COPI;
	PinNoInit CIPO;
	PinNoInit CS[NUM_CHIPS];

	bool use_hardware_ss;
	uint16_t clock_division;
	uint16_t data_size;
	SpiDataDir data_dir;
};

struct DefaultSpiConf {
	static constexpr uint16_t PeriphNum = 1; // SPI1
	static constexpr uint16_t NumChips = 1;
	static constexpr uint16_t IRQn = SPI1_IRQn;
	static constexpr uint16_t priority1 = 3;
	static constexpr uint16_t priority2 = 3;
	static constexpr PinNoInit SCLK = {GPIO::A, 1, LL_GPIO_AF_0};
	static constexpr PinNoInit COPI = {GPIO::A, 1, LL_GPIO_AF_0};
	static constexpr PinNoInit CIPO = {GPIO::A, 1, LL_GPIO_AF_0};
	static constexpr PinNoInit CS[2] = {
		{GPIO::A, 1},
	};
	static constexpr bool use_hardware_ss = false;
	static constexpr uint16_t clock_division = 64;
	static constexpr uint16_t data_size = 16;
};

