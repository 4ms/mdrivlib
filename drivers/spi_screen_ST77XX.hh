#pragma once
#include "util/math.hh"
#include <array>
#include <cstdint>

namespace mdrivlib
{

namespace ST77XX
{
enum Command : uint8_t {
	ST_CMD_DELAY = 0x80,

	NOP = 0x00,
	SWRESET = 0x01,
	RDDID = 0x04,
	RDDST = 0x09,

	SLPIN = 0x10,
	SLPOUT = 0x11,
	PTLON = 0x12,
	NORON = 0x13,

	INVOFF = 0x20,
	INVON = 0x21,
	DISPOFF = 0x28,
	DISPON = 0x29,
	CASET = 0x2A,
	RASET = 0x2B,
	RAMWR = 0x2C,
	RAMRD = 0x2E,

	PTLAR = 0x30,
	TEOFF = 0x34,
	TEON = 0x35,
	MADCTL = 0x36,
	COLMOD = 0x3A,

	WRDISBV = 0x51,
	WRCTRLD = 0x53,

	RAMCTRL = 0xB0,
	RGBCTRL = 0xB1,
	PORCTRL = 0xB2,
};

enum Arg {
	MADCTL_MY = 0x80,
	MADCTL_MX = 0x40,
	MADCTL_MV = 0x20,
	MADCTL_ML = 0x10,
	MADCTL_RGB = 0x00,

	RDID1 = 0xDA,
	RDID2 = 0xDB,
	RDID3 = 0xDC,
	RDID4 = 0xDD,
};

enum MADCTL_Rotation {
	MADCTL_ROT0 = ST77XX::MADCTL_MX | ST77XX::MADCTL_MY,
	MADCTL_ROTCW90 = ST77XX::MADCTL_MY | ST77XX::MADCTL_MV,
	MADCTL_ROT180 = 0,
	MADCTL_ROTCCW90 = ST77XX::MADCTL_MX | ST77XX::MADCTL_MV,
};

enum COLMOD_Args {
	COLMOD_16B = 0x55,
};

enum RAMCTRL_Args {
	RAMCTRL_RGB_IF = 0b00010001,
	RAMCTRL_EPF_DEFAULT = 0xF0,
	RAMCTRL_MDT_16B_16B = 0b00,
};

struct InitCommand {
	ST77XX::Command cmd;
	uint8_t num_args;
	uint16_t delay_ms;
	uint32_t args;

	static constexpr uint32_t makecmd(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
		return (d << 24) | (c << 16) | (b << 8) | a;
	}
	static constexpr uint32_t makecmd(uint16_t a, uint16_t b) {
		return MathTools::swap_bytes_and_combine(b, a);
	}
};

enum InvertState { NotInverted, Inverted };

constexpr uint32_t make_RGBCTRL_args(uint8_t v_back_porch, uint8_t h_back_porch) {
	constexpr uint8_t all_polarities_low = 0;
	constexpr uint8_t de_mode = 0b10 << 5;
	constexpr uint8_t use_ram = 0b00 << 7;
	return InitCommand::makecmd(all_polarities_low | de_mode | use_ram, v_back_porch & 127, h_back_porch & 31, 0);
}

// TODO: incorporate Rotation into the MADCTL commands
template<uint16_t WIDTH, uint16_t HEIGHT, enum InvertState ISINVERTED>
struct ST7789Init {
	static constexpr InitCommand cmds[] =
		{
			//  1: Software reset, no args, w/delay
			{SWRESET, 0, 150},

			//  2: Out of sleep mode, no args, w/delay
			{SLPOUT, 0, 10},

			//  3: Set color mode, 1 arg, 10ms delay, 0x55 = 16-bit color
			{COLMOD, 1, 10, 0x55},

			//  4: Mem access ctrl (directions),
			//	 	0x08 =  Row/col addr, bottom-top refresh, BGR not RGB
			{MADCTL, 1, 0, 0x08},

			//  5: Column addr set, 4 args, no delay
			// 		XSTART = 0, XEND = WIDTH
			{CASET, 4, 0, InitCommand::makecmd(0, WIDTH)},

			//  6: Row addr set, 4 args, no delay:
			// 		YSTART = 0, YEND = HEIGHT
			{RASET, 4, 0, InitCommand::makecmd(0, HEIGHT)},

			//  7: Inverted or not
			{ISINVERTED == Inverted ? INVON : INVOFF, 0, 10},

			// Display Brightness Max
			// {.cmd = WRCTRLD, .num_args = 1, .delay_ms = 0, .args = 0b00101100},
			// {.cmd = WRDISBV, .num_args = 1, .delay_ms = 0, .args = 0xFF0000FF},

			//  8: Normal display on, no args, w/delay
			{NORON, 0, 10},

			//  9: Main screen turn on, no args, delay
			{DISPON, 0, 10},

		};

	static constexpr uint32_t num_commands = sizeof(cmds) / sizeof(InitCommand);
};

template<typename ConfT>
struct ST7789RGBInit {
	static constexpr std::array<InitCommand, 12> cmds = {
		//  1: Software reset, no args, w/delay
		InitCommand{SWRESET, 0, 150},

		//  2: Out of sleep mode, no args, w/delay
		{SLPOUT, 0, 10},

		//  3: Set color mode, 1 arg, 10ms delay, 0x55 = 16-bit color
		{COLMOD, 1, 10, COLMOD_16B},

		//  4: Mem access ctrl (directions),
		{MADCTL,
		 1,
		 0,
		 ConfT::rotation == ConfT::NoRotation ? MADCTL_ROT0
		 : ConfT::rotation == ConfT::CW90	  ? MADCTL_ROTCW90
		 : ConfT::rotation == ConfT::Flip180  ? MADCTL_ROT180
											  : MADCTL_ROTCCW90},

		//  5: Column addr set, 4 args, no delay
		// 		XSTART = 0, XEND = WIDTH
		{CASET, 4, 0, InitCommand::makecmd(0, ConfT::width)},

		//  6: Row addr set, 4 args, no delay:
		// 		YSTART = 0, YEND = HEIGHT
		{RASET, 4, 0, InitCommand::makecmd(0, ConfT::height)},

		//  7: Inverted or not
		{ConfT::IsInverted == Inverted ? INVON : INVOFF, 0, 10},

		// RAM Control: set RGB mode. EPF and MTD don't matter
		{RAMCTRL, 2, 0, InitCommand::makecmd(RAMCTRL_RGB_IF, RAMCTRL_EPF_DEFAULT, 0, 0)},

		// RGB Control: V/H back porch
		{RGBCTRL, 3, 0, make_RGBCTRL_args(ConfT::VBackPorch, ConfT::HBackPorch)},

		// Porch setting: Not sure if this wants V or H porch?
		{PORCTRL, 3, 0, InitCommand::makecmd(ConfT::HBackPorch, ConfT::HFrontPorch, 0, 0)},

		// Normal display on, no args, w/delay
		{NORON, 0, 10},

		// Main screen turn on, no args, delay
		{DISPON, 0, 10},

	};
};

} // namespace ST77XX
} // namespace mdrivlib
