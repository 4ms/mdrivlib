#pragma once
#include "util/math.hh"
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
} // namespace ST77XX

// TODO: incorporate Rotation into the MADCTL commands
template<uint16_t WIDTH, uint16_t HEIGHT, enum ST77XX::InvertState ISINVERTED>
struct ST7789Init {
	static constexpr uint32_t num_commands = 9;
	static constexpr ST77XX::InitCommand cmds[num_commands] = {
		//  1: Software reset, no args, w/delay
		{ST77XX::SWRESET, 0, 150},

		//  2: Out of sleep mode, no args, w/delay
		{ST77XX::SLPOUT, 0, 10},

		//  3: Set color mode, 1 arg, 10ms delay, 0x55 = 16-bit color
		{ST77XX::COLMOD, 1, 10, 0x55},

		//  4: Mem access ctrl (directions),
		//	 	0x08 =  Row/col addr, bottom-top refresh, BGR not RGB
		{ST77XX::MADCTL, 1, 0, 0x08},

		//  5: Column addr set, 4 args, no delay
		// 		XSTART = 0, XEND = WIDTH
		{ST77XX::CASET, 4, 0, ST77XX::InitCommand::makecmd(0, WIDTH)},

		//  6: Row addr set, 4 args, no delay:
		// 		YSTART = 0, YEND = HEIGHT
		{ST77XX::RASET, 4, 0, ST77XX::InitCommand::makecmd(0, HEIGHT)},

		//  7: Inverted or not
		{ISINVERTED == ST77XX::Inverted ? ST77XX::INVON : ST77XX::INVOFF, 0, 10},

		//  8: Normal display on, no args, w/delay
		{ST77XX::NORON, 0, 10},

		//  9: Main screen turn on, no args, delay
		{ST77XX::DISPON, 0, 10},

	};
};

} // namespace mdrivlib
