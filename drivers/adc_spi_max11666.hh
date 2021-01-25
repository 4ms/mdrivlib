#pragma once
#include "drivers/pin.hh"
#include "stm32xx.h"

// spi.hh:
#include "interrupt.hh"

// template<unsigned NUM_CHIPS = 1>
// struct SpiPins {
// 	PinNoInit SCLK;
// 	PinNoInit COPI;
// 	PinNoInit CIPO;
// 	PinNoInit CS[NUM_CHIPS];
// };

template<unsigned NUM_CHIPS = 1>
struct SpiConfig {
	SPI_TypeDef *spi;
	IRQType IRQn;

	PinNoInit SCLK;
	PinNoInit COPI;
	PinNoInit CIPO;
	PinNoInit CS[NUM_CHIPS];

	// SpiPins<NUM_CHIPS> pins;

	// unsigned num_chips;
	// unsigned chans_per_chip;
	// uint32_t change_channel_commands[NUM_CHIPS];
	// uint32_t continue_channel_commands[NUM_CHIPS];
};

template<unsigned NUM_CHIPS = 1>
class SpiPeriph {
public:
	SpiPeriph(const SpiConfig<NUM_CHIPS> &conf)
		: _spi{conf.spi} {}

	void init() {
		Pin sclk{_conf.SCLK};
		Pin copi{_conf.COPI};
		Pin cipo{_conf.CIPO};
		for (int i = 0; i < NUM_CHIPS; i++)
			CS[i].init(_conf.CS[i]);
	}
	void init_IT() {}
	void enable() {}
	void disable() {}
	void send(uint16_t data) { _spi->TXDR = data; }
	void select(int chip_num) { CS[chip_num].low(); }
	void unselect(int chip_num) { CS[chip_num].high(); }

private:
	SPI_TypeDef *_spi;
	const SpiConfig<NUM_CHIPS> &_conf;
	Pin CS[NUM_CHIPS];
};
// end spi.hh

template<unsigned NUM_CHIPS = 1>
struct AdcSpi_MAX11666 {
	AdcSpi_MAX11666(const SpiConfig<NUM_CHIPS> &conf)
		: spi{conf} {
		InterruptManager::registerISR(conf.IRQn, [this]() {
			// Handle ISR here
		});
	}
	void start() {}

	uint16_t get_val(int chan) { return val[chan]; }
	void advance_channel() {}

private:
	SpiPeriph<NUM_CHIPS> spi;
	uint16_t val[NUM_CHIPS * 2];
};

