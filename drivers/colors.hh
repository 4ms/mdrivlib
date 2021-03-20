#pragma once
#include "stm32xx.h"

#if (defined(__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1))
#else
#define __UQADD8(x, y) ((x) + (y))
#endif

struct Color {
	// Todo: measure how much slower it is to use floats for Adjustment.r/b/g
	struct Adjustment {
		uint8_t r, g, b;
	};

	explicit constexpr Color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0)
		: r_(r)
		, g_(g)
		, b_(b) {
	}

	constexpr Color(uint16_t rgb565)
		: r_((rgb565 & 0xf800) >> 8)
		, g_((rgb565 & 0x07e0) >> 3)
		, b_((rgb565 & 0x001f) << 3) {
	}

	constexpr uint8_t red() const {
		return r_;
	}
	constexpr uint8_t green() const {
		return g_;
	}
	constexpr uint8_t blue() const {
		return b_;
	}

	const Color operator+(Color const that) const {
		return Color(__UQADD8(r_, that.r_), __UQADD8(g_, that.g_), __UQADD8(b_, that.b_));
	}

	Color &operator=(Color const that) {
		r_ = that.r_;
		g_ = that.g_;
		b_ = that.b_;
		return *this;
	}

	constexpr Color blend(Color const that) const {
		return Color((r_ >> 1) + (that.r_ >> 1), (g_ >> 1) + (that.g_ >> 1), (b_ >> 1) + (that.b_ >> 1));
	}

	// Todo: faster blend, no division
	constexpr const Color blend(Color const that, uint8_t const phase) const {
		return Color((r_ * (255 - phase) + that.r_ * phase) / 255,	//>> 8,
					 (g_ * (255 - phase) + that.g_ * phase) / 255,	//>> 8,
					 (b_ * (255 - phase) + that.b_ * phase) / 255); //>> 8);
	}
	constexpr const Color blend(Color const that, uint32_t const phase) const {
		return Color((r_ * (256 - (phase >> 24)) + that.r_ * (phase >> 24)) >> 8,
					 (g_ * (256 - (phase >> 24)) + that.g_ * (phase >> 24)) >> 8,
					 (b_ * (256 - (phase >> 24)) + that.b_ * (phase >> 24)) >> 8);
	}
	constexpr const Color blend(Color const that, float const phase) const {
		return Color((r_ * (1.0f - phase) + that.r_ * phase),
					 (g_ * (1.0f - phase) + that.g_ * phase),
					 (b_ * (1.0f - phase) + that.b_ * phase));
	}
	constexpr const bool operator!=(Color const that) {
		return this->r_ != that.r_ || this->g_ != that.g_ || this->b_ != that.b_;
	}

	constexpr const Color adjust(Adjustment const adj) const {
		// uint16_t ra = r_ * adj.r;
		// uint16_t rg = g_ * adj.g;
		// uint16_t rb = b_ * adj.b;
		// return Color(ra >> 7, rg >> 7, rb >> 7);
		return Color((r_ * adj.r) >> 7, (g_ * adj.g) >> 7, (b_ * adj.b) >> 7);
	}

	constexpr const uint16_t Rgb565() const {
		return ((r_ & 0b11111000) << 8) | ((g_ & 0b11111100) << 3) | ((b_ >> 3));
	}

	static constexpr uint16_t blend(const uint16_t color1, const uint16_t color2, const float f_alpha) {
		const uint8_t alpha = f_alpha * 255.f;
		Color c1{color1};
		Color c2{color2};
		return c1.blend(c2, alpha).Rgb565();
	}

private:
	uint8_t r_, g_, b_;
};

struct Colors {
	static constexpr Color off = Color(0, 0, 0);
	static constexpr Color black = Color(0, 0, 0);
	static constexpr Color grey = Color(127, 127, 127);
	static constexpr Color white = Color(255, 255, 255);

	static constexpr Color red = Color(255, 0, 0);
	static constexpr Color pink = Color(0xFF, 0x69, 0xB4);
	static constexpr Color pale_pink = Color(255, 200, 200);
	static constexpr Color tangerine = Color(255, 200, 0);
	static constexpr Color orange = Color(255, 127, 0);
	static constexpr Color yellow = Color(255, 255, 0);
	static constexpr Color green = Color(0, 255, 0);
	static constexpr Color cyan = Color(0, 255, 255);
	static constexpr Color blue = Color(0, 0, 255);
	static constexpr Color purple = Color(255, 0, 255);
	static constexpr Color magenta = Color(200, 0, 100);
};
