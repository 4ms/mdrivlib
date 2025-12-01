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
struct DebouncedButton : public DebouncerCounter<RisingEdgePattern, FallingEdgePattern, StateMask> {

	using PinT = FPin<Pindef.gpio, Pindef.pin, PinMode::Input, Polarity>;

	DebouncedButton() {
		PinT init_pin{Polarity == PinPolarity::Inverted ? PinPull::Up : PinPull::None};
	}

	void update() {
		unsigned pin_state = PinT::read() ? 1 : 0;
		this->register_state(pin_state);
	}

	unsigned how_long_held() {
		return this->steady_state_ctr;
	}

	unsigned how_long_held_pressed() {
		return this->is_pressed() ? this->steady_state_ctr : 0;
	}

	void reset_hold_ctr() {
		this->steady_state_ctr = 0;
	}
};

//
// DebouncedSPDT3pos: SPDT switch with center off
//
template<PinDef Pin0def,
		 PinDef Pin1def,
		 PinPolarity Polarity, //Normal = pole is V+. Inverted = pole is GND
		 unsigned RisingEdgePattern = 0x00000001,
		 unsigned FallingEdgePattern = 0xFFFFFFFE,
		 unsigned StateMask = 0x00000FFF>
struct DebouncedSPDT3pos {
	Debouncer<RisingEdgePattern, FallingEdgePattern, StateMask> debouncer0;
	Debouncer<RisingEdgePattern, FallingEdgePattern, StateMask> debouncer1;

	using Pin0T = FPin<Pin0def.gpio, Pin0def.pin, PinMode::Input, Polarity>;
	using Pin1T = FPin<Pin1def.gpio, Pin1def.pin, PinMode::Input, Polarity>;

	DebouncedSPDT3pos() {
		Pin0T init_pin0{Polarity == PinPolarity::Inverted ? PinPull::Up : PinPull::None};
		Pin1T init_pin1{Polarity == PinPolarity::Inverted ? PinPull::Up : PinPull::None};
	}

	void update() {
		unsigned pin0_state = Pin0T::read() ? 1 : 0;
		debouncer0.register_state(pin0_state);
		unsigned pin1_state = Pin1T::read() ? 1 : 0;
		debouncer1.register_state(pin1_state);
	}

	enum Pos { Low = 0b01, Center = 0b00, High = 0b10, Invalid = 0b11 };

	Pos state() {
		return (debouncer0.is_high() ? 0b01 : 0) | (debouncer1.is_high() ? 0b10 : 0);
	}
};

} // namespace mdrivlib
