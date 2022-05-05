#pragma once
#include "pin.hh"
#include "util/debouncer.hh"
#include <cstdint>

namespace mdrivlib
{
template<GPIO Gpio,
		 uint16_t PinNum,
		 PinPolarity Polarity,
		 unsigned RisingEdgePattern = 0x00000001,
		 unsigned FallingEdgePattern = 0xFFFFFFFE,
		 unsigned StateMask = 0x00000FFF>
struct DebouncedPin : public Debouncer<RisingEdgePattern, FallingEdgePattern, StateMask> {
	DebouncedPin() {
		FPin<Gpio, PinNum, PinMode::Input, Polarity> init_pin{Polarity == PinPolarity::Inverted ? PinPull::Up
																								: PinPull::None};
	}

	void update() {
		unsigned pin_state = FPin<Gpio, PinNum, PinMode::Input, Polarity>::read() ? 1 : 0;
		this->register_state(pin_state);
	}
};

template<GPIO Gpio, uint16_t PinNum, PinPolarity Polarity>
using DebouncedSwitch = DebouncedPin<Gpio, PinNum, Polarity>;

} // namespace mdrivlib
