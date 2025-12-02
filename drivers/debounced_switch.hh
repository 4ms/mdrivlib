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
template<PinDef PinRightDef,
		 PinDef PinLeftDef,
		 PinPolarity Polarity, //Normal = pole is V+. Inverted = pole is GND
		 unsigned RisingEdgePattern = 0x00000001,
		 unsigned FallingEdgePattern = 0xFFFFFFFE,
		 unsigned StateMask = 0x00000FFF>
struct DebouncedSPDT3pos {
	Debouncer<RisingEdgePattern, FallingEdgePattern, StateMask> debouncer_right;
	Debouncer<RisingEdgePattern, FallingEdgePattern, StateMask> debouncer_left;

	using PinRightT = FPin<PinRightDef.gpio, PinRightDef.pin, PinMode::Input, Polarity>;
	using PinLeftT = FPin<PinLeftDef.gpio, PinLeftDef.pin, PinMode::Input, Polarity>;

	DebouncedSPDT3pos() {
		PinRightT init_pin_right{Polarity == PinPolarity::Inverted ? PinPull::Up : PinPull::None};
		PinLeftT init_pin_left{Polarity == PinPolarity::Inverted ? PinPull::Up : PinPull::None};
	}

	void update() {
		unsigned pin_right_state = PinRightT::read() ? 1 : 0;
		debouncer_right.register_state(pin_right_state);
		unsigned pin_left_state = PinLeftT::read() ? 1 : 0;
		debouncer_left.register_state(pin_left_state);
	}

	enum Pos { Left = 0b01, Center = 0b00, Right = 0b10, Invalid = 0b11 };

	Pos state() {
		return static_cast<Pos>((debouncer_right.is_high() ? 0b10 : 0) | (debouncer_left.is_high() ? 0b01 : 0));
	}
};

} // namespace mdrivlib
