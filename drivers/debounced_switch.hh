#pragma once
#include "pin.hh"
#include "util/debouncer.hh"
#include <cstdint>

namespace mdrivlib
{
template<GPIO Gpio, uint16_t PinNum, PinPolarity Polarity>
struct DebouncedPin : public Debouncer<0x00000001, 0xFFFFFFFE, 0x00000FFF> {
	DebouncedPin() {
		FPin<Gpio, PinNum, PinMode::Input, Polarity> init_pin{Polarity == PinPolarity::Inverted ? PinPull::Up
																								: PinPull::None};
	}

	void update() {
		unsigned pin_state = FPin<Gpio, PinNum, PinMode::Input, Polarity>::read() ? 1 : 0;
		register_state(pin_state);
	}
};

template<GPIO Gpio, uint16_t PinNum, PinPolarity Polarity>
using DebouncedSwitch = DebouncedPin<Gpio, PinNum, Polarity>;

} // namespace mdrivlib
