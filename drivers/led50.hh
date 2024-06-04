#pragma once

#include "drivers/pin.hh"

namespace mdrivlib
{

//LED where value > 50% turns it on, otherwise off
template<GPIO Gpio, uint8_t PinNum, PinPolarity Polarity = PinPolarity::Normal>
struct Led50 : FPin<Gpio, PinNum, PinMode::Output, Polarity> {
	static void set(uint8_t val) {
		FPin<Gpio, PinNum, PinMode::Output, Polarity>::set(val > 0x7F);
	}
};

} // namespace mdrivlib
