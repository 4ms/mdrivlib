#pragma once
#include "interrupt.hh"
#include "pin.hh"

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
};
