#pragma once
#include "pin.hh"

namespace mdrivlib
{

struct LTDCScreenConf {
	static constexpr PinDef r[8]{};
	static constexpr PinDef g[8]{};
	static constexpr PinDef b[8]{};
	static constexpr PinDef de{};
	static constexpr PinDef clk{};
	static constexpr PinDef vsync{};
	static constexpr PinDef hsync{};

	static constexpr uint32_t width = 240;
	static constexpr uint32_t height = 240;
	static constexpr uint32_t rowstart = 80;
	static constexpr uint32_t colstart = 0;

	enum Rotation { NoRotation, CW90, Flip180, CCW90 };
	static constexpr Rotation rotation = NoRotation;

	enum ColorFormat { RGBA8888, RGB888, RGB565 };
	static constexpr ColorFormat color_format = RGB565;

	static constexpr uint32_t HSyncWidth = 0;
	static constexpr uint32_t HBackPorch = 0;
	static constexpr uint32_t HFrontPorch = 0;
	static constexpr uint32_t VSyncWidth = 0;
	static constexpr uint32_t VBackPorch = 0;
	static constexpr uint32_t VFrontPorch = 0;
};

} // namespace mdrivlib
