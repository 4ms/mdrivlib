#pragma once
#include "pin.hh"
#include "util/debouncer.hh"
#include <cstdint>

namespace mdrivlib
{
template<PinDef Pindef,
		 PinPolarity Polarity,
		 unsigned RisingEdgePattern = 0x00000001,
		 unsigned FallingEdgePattern = 0xFFFFFFFE,
		 unsigned StateMask = 0x00000FFF>
struct DebouncedPin : public Debouncer<RisingEdgePattern, FallingEdgePattern, StateMask> {

	using PinT = FPin<Pindef.gpio, Pindef.pin, PinMode::Input, Polarity>;

	DebouncedPin() {
		PinT init_pin{Polarity == PinPolarity::Inverted ? PinPull::Up : PinPull::None};
	}

	void update() {
		unsigned pin_state = PinT::read() ? 1 : 0;
		this->register_state(pin_state);
	}
};

template<PinDef Pindef, PinPolarity Polarity>
using DebouncedSwitch = DebouncedPin<Pindef, Polarity>;

//
// DebouncedButton: same as DebouncedSwitch/Pin but counts how long is held down or up
//
template<PinDef Pindef,
		 PinPolarity Polarity,
		 unsigned RisingEdgePattern = 0x00000001,
		 unsigned FallingEdgePattern = 0xFFFFFFFE,
		 unsigned StateMask = 0x00000FFF>
struct DebouncedButton : DebouncerCounter<RisingEdgePattern, FallingEdgePattern, StateMask> {
	using PinT = FPin<Pindef.gpio, Pindef.pin, PinMode::Input, Polarity>;

	void update() {
		unsigned pin_state = PinT::read() ? 1 : 0;
		this->register_state(pin_state);
	}

	unsigned how_long_held() {
		return this->steady_state_ctr;
	}

	void reset_hold_ctr() {
		this->steady_state_ctr = 0;
	}
};

} // namespace mdrivlib
