#pragma once
#include "pin.hh"
#include "stm32xx.h"

namespace mdrivlib
{

struct LTDCScreenConf {
	static constexpr PinNoInit r[8]{};
	static constexpr PinNoInit g[8]{};
	static constexpr PinNoInit b[8]{};
	static constexpr PinNoInit de{};
	static constexpr PinNoInit clk{};
	static constexpr PinNoInit vsync{};
	static constexpr PinNoInit hsync{};

	static constexpr uint32_t width = 240;
	static constexpr uint32_t height = 240;
	static constexpr uint32_t rowstart = 80;
	static constexpr uint32_t colstart = 0;

	enum Rotation { None, CW90, Flip180, CCW90 };
	static constexpr Rotation rotation = None;

	enum ColorFormat { RGBA8888, RGB888, RGB565 };
	static constexpr ColorFormat color_format = RGB565;

	static constexpr uint32_t HSyncWidth = 0;
	static constexpr uint32_t HBackPorch = 0;
	static constexpr uint32_t HFrontPorch = 0;
	static constexpr uint32_t VSyncWidth = 0;
	static constexpr uint32_t VBackPorch = 0;
	static constexpr uint32_t VFrontPorch = 0;
};

struct LTDCScreenControlConf {
	static constexpr PinNoInit cs{};
	static constexpr PinNoInit serial_data_out{};
	static constexpr PinNoInit serial_write{};
	static constexpr PinNoInit serial_datacmd_sel{};

	using ScreenConf = LTDCScreenConf;
};
} // namespace mdrivlib
