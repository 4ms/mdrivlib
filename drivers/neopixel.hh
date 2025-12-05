#pragma once
#include "drivers/dma_transfer.hh"
#include "drivers/tim_pwm.hh"
#include <array>

namespace mdrivlib
{

template<TimChanConf Conf, typename DMAConf, size_t NumLeds>
struct NeoPixel {
	NeoPixel() {
		pwm.init();
		// /4 =>  150us off, 480us pulses
		// each led is 24 pwm bits. Each pwm bit is 1.25us.
		// so led time is 40 * 24 * 1.25us = 1200us = 1.2ms
		// but we only have 482us
		dma.config_transfer(const_cast<uint32_t *>(pwm.get_CCR()), &bits, sizeof(bits));

		// Set the reset pattern:
		for (auto i = NumLeds; i < bits.size(); i++) {
			bits[i].g.fill(PWM_RESET);
			bits[i].r.fill(PWM_RESET);
			bits[i].b.fill(PWM_RESET);
		}

		all_leds_off();
		printf("#LEDs = %u. Bits is %zu\n", NumLeds, sizeof(bits));
	}

	void start() {
		pwm.enable_dma_mode();
		dma.start_transfer();
	}

	void all_leds_off() {
		for (auto i = 0u; i < NumLeds; i++) {
			bits[i].g.fill(PWM_LO);
			bits[i].r.fill(PWM_LO);
			bits[i].b.fill(PWM_LO);
		}
	}

	void set_led(unsigned led, uint8_t r, uint8_t g, uint8_t b) {
		if (led >= NumLeds)
			return;
		for (auto i = 0; i < 8; i++) {
			bits[led].g[i] = ((g << i) & 0x80) ? PWM_HI : PWM_LO;
			bits[led].r[i] = ((r << i) & 0x80) ? PWM_HI : PWM_LO;
			bits[led].b[i] = ((b << i) & 0x80) ? PWM_HI : PWM_LO;
		}
	}

	// Each bit is represented by a PWM code (which is stored as a u16)
	struct ColorBits {
		std::array<uint16_t, 8> g{};
		std::array<uint16_t, 8> r{};
		std::array<uint16_t, 8> b{};
	};

	// Reset code must be >= 80us of low == 64 PWM bits == 2.67 Leds
	static constexpr unsigned ResetPeriods = 3; //90us measured
	std::array<ColorBits, NumLeds + ResetPeriods> bits{};

private:
	// 1.25us total cycle time

	// Bit High: 0.45us - 0.75s pulse (0.6us target)
	static constexpr uint16_t PWM_HI = (Conf.period + 1) / 2;

	// Bit Low: 0.15us - 0.45us pulse (0.3us target)
	static constexpr uint16_t PWM_LO = (Conf.period + 1) * (TimPwmChan<Conf>::is_inverted() ? 0.76f : 0.24f);

	static constexpr uint16_t PWM_RESET = 0; //Conf.period + 1;

	TimPwmChan<Conf> pwm;
	DMATransfer<DMAConf> dma;
};
} // namespace mdrivlib
