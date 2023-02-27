#pragma once
#include "pin.hh"

namespace mdrivlib
{

enum class SwitchPos { Invalid = 0b00, Up = 0b01, Down = 0b10, Center = 0b11 };

template<PinDef UpPin, PinDef DownPin>
struct Switch3Pos {

	using UpPinT = FPin<UpPin.gpio, UpPin.pin, PinMode::Input, PinPolarity::Normal>;
	using DownPinT = FPin<DownPin.gpio, DownPin.pin, PinMode::Input, PinPolarity::Normal>;

	Switch3Pos() {
		UpPinT init_up_pin{PinPull::Up};
		DownPinT init_down_pin{PinPull::Up};
	}

	SwitchPos read() {
		unsigned pin_state = UpPinT::read() ? 0b10 : 0b00;
		pin_state |= DownPinT::read() ? 1 : 0;
		return static_cast<SwitchPos>(pin_state);
	}
};

} // namespace mdrivlib
