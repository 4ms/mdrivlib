#pragma once
#include "drivers/dma_transfer.hh"
#include "drivers/tim_pwm.hh"
#include <array>

namespace mdrivlib
{

template<TimChanConf Conf, typename DMAConf, size_t NumLeds>
struct NeoPixel {
	constexpr static bool is_inv_channel() {
		return (Conf.channum == TimChannelNum::_1N || Conf.channum == TimChannelNum::_2N ||
				Conf.channum == TimChannelNum::_3N);
	}

	NeoPixel() {
		pwm.init();

		if constexpr (is_inv_channel())
			pwm.downcount_mode();
		else
			pwm.upcount_mode();

		dma.config_transfer(const_cast<uint32_t *>(pwm.get_CCR()), &bits, sizeof(bits));

		// Fill the reset pattern before and after the LED buffer:
		for (auto i = NumLeds; i < bits.size(); i++) {
			bits[i].g.fill(PWM_RESET);
			bits[i].r.fill(PWM_RESET);
			bits[i].b.fill(PWM_RESET);
		}
		for (auto i = 0u; i < ResetPeriods; i++) {
			bits[i].g.fill(PWM_RESET);
			bits[i].r.fill(PWM_RESET);
			bits[i].b.fill(PWM_RESET);
		}

		all_leds_off();
		pwm.enable_dma_mode();
	}

	void start() {
		pwm.stop_timer();
		dma.start_transfer();
		pwm.start_timer();
	}

	void all_leds_off() {
		for (auto i = 0u; i < NumLeds; i++) {
			auto l = i + ResetPeriods;
			bits[l].g.fill(PWM_LO);
			bits[l].r.fill(PWM_LO);
			bits[l].b.fill(PWM_LO);
		}
	}

	void all_leds_on() {
		for (auto i = 0u; i < NumLeds; i++) {
			auto l = i + ResetPeriods;
			bits[l].g.fill(PWM_HI);
			bits[l].r.fill(PWM_HI);
			bits[l].b.fill(PWM_HI);
		}
	}

	void all_leds_rgb(uint8_t r, uint8_t g, uint8_t b) {
		for (auto led = 0u; led < NumLeds; led++) {
			for (auto i = 0; i < 8; i++) {
				auto l = led + ResetPeriods;
				bits[l].g[i] = ((g << i) & 0x80) ? PWM_HI : PWM_LO;
				bits[l].r[i] = ((r << i) & 0x80) ? PWM_HI : PWM_LO;
				bits[l].b[i] = ((b << i) & 0x80) ? PWM_HI : PWM_LO;
			}
		}
	}

	void set_led(unsigned led, uint8_t r, uint8_t g, uint8_t b) {
		if (led >= NumLeds)
			return;
		led += ResetPeriods;
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

	// Reset code must be >= 80us of low == 64 PWM bits == 2.67 Leds => round up to 3
	static constexpr unsigned ResetPeriods = 3; //90us measured
	std::array<ColorBits, NumLeds + ResetPeriods * 2> bits{};

private:
	// 1.25us total cycle time (800kHz)

	// Bit High: 0.45us - 0.75s pulse (0.6us target)
	static constexpr uint16_t PWM_HI = (Conf.period + 1) / 2;

	// Bit Low: 0.15us - 0.45us pulse (0.3us target)
	static constexpr uint16_t PWM_LO = (Conf.period + 1) * 0.76f;

	static constexpr uint16_t PWM_RESET = 0xFFFF;

	TimPwmChan<Conf> pwm;
	DMATransfer<DMAConf> dma;
};
} // namespace mdrivlib
