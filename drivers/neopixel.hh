#pragma once
#include "drivers/tim_pwm.hh"
#include <array>

namespace mdrivlib
{

template<TimChanConf Conf, size_t NumLeds>
struct NeoPixel {
	NeoPixel() {
		pwm.init();
	}

	void set_led(unsigned led, uint8_t r, uint8_t g, uint8_t b) {
		vals[led * 3] = g;
		vals[led * 3 + 1] = r;
		vals[led * 3 + 2] = b;
	}

	// 24-bit color
	std::array<uint8_t, NumLeds * 3> vals{};
	TimPwmChan<Conf> pwm;
};
} // namespace mdrivlib
