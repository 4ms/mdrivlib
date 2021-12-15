#pragma once
#include "drivers/screen_ltdc_setup.hh"
#include "util/math.hh"
#include <array>
#include <cstdint>
#include <span>

namespace mdrivlib::ST77XX
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
	IDMOFF = 0x38,
	COLMOD = 0x3A,

	WRDISBV = 0x51,
	WRCTRLD = 0x53,

	RAMCTRL = 0xB0,
	RGBCTRL = 0xB1,
	PORCTRL = 0xB2,
	GCTRL = 0xB8,

	FRCTRL2 = 0xC6,
	CABCCTRL = 0xC7,
	PWMFRSEL = 0xCC,

	CMD2EN = 0xDF,
};

enum MADCTL_Arg {
	MADCTL_MY = 0x80,
	MADCTL_MX = 0x40,
	MADCTL_MV = 0x20,
	MADCTL_ML = 0x10,
	MADCTL_RGB = 0x00,
};

enum MADCTL_Rotation {
	MADCTL_ROT0 = ST77XX::MADCTL_MX | ST77XX::MADCTL_MY,
	MADCTL_ROTCW90 = ST77XX::MADCTL_MY | ST77XX::MADCTL_MV,
	MADCTL_ROT180 = 0,
	MADCTL_ROTCCW90 = ST77XX::MADCTL_MX | ST77XX::MADCTL_MV,
};

enum RDID_Arg {
	RDID1 = 0xDA,
	RDID2 = 0xDB,
	RDID3 = 0xDC,
	RDID4 = 0xDD,
};

enum COLMOD_Args {
	COLMOD_16B = 0x55,
};

enum RAMCTRL_Args {
	RAMCTRL_RGB_IF = 0b00010001,
	RAMCTRL_EPF_DEFAULT = 0xF0,
	RAMCTRL_MDT_16B_16B = 0b00,
};

enum RGBCTRL_Args {
	//RGBCTRL_AllPolaritiesLow = 0,
	RGBCTRL_DE_Mode = 0b10 << 5,
	RGBCTRL_HV_Mode = 0b11 << 5,
	RGBCTRL_UseShiftReg = 1 << 7,
	RGBCTRL_UseRAM = 0 << 7,
};

enum InvertState { NotInverted, Inverted };

template<uint16_t WIDTH, uint16_t HEIGHT, enum InvertState ISINVERTED>
struct ST7789Init {
	static constexpr ScreenInitCommand cmds[] =
		{
			//  1: Software reset, no args, w/delay
			{SWRESET, 0, 150},

			//  2: Out of sleep mode, no args, w/delay
			{SLPOUT, 0, 10},

			//  3: Set color mode, 1 arg, 10ms delay, 0x55 = 16-bit color
			{COLMOD, 1, 10, {0x55}},

			//  4: Mem access ctrl (directions),
			//	 	0x08 =  Row/col addr, bottom-top refresh, BGR not RGB
			{MADCTL, 1, 0, {0x08}},

			//  5: Column addr set, 4 args, no delay
			// 		XSTART = 0, XEND = WIDTH
			{CASET, 4, 0, {0, WIDTH >> 8, 0, WIDTH & 0xFF}},

			//  6: Row addr set, 4 args, no delay:
			// 		YSTART = 0, YEND = HEIGHT
			{RASET, 4, 0, {0, HEIGHT >> 8, 0, HEIGHT & 0xFF}},

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

	static constexpr uint32_t num_commands = sizeof(cmds) / sizeof(ScreenInitCommand);
};

template<typename ConfT>
struct ST7789InitLTDC {
	static constexpr ScreenInitCommand cmds[] = {
		{.cmd = SLPIN, .num_args = 0, .delay_ms = 10},
		{.cmd = SWRESET, .num_args = 0, .delay_ms = 150},
		{.cmd = SLPOUT, .num_args = 0, .delay_ms = 120},

		{.cmd = MADCTL,
		 .num_args = 1,
		 .delay_ms = 0,
		 .args = {ConfT::rotation == ConfT::NoRotation ? MADCTL_ROT0
				  : ConfT::rotation == ConfT::CW90	   ? MADCTL_ROTCW90
				  : ConfT::rotation == ConfT::Flip180  ? MADCTL_ROT180
													   : MADCTL_ROTCCW90}},

		{.cmd = COLMOD, .num_args = 1, .delay_ms = 0, .args = {0x55}},

		{.cmd = ConfT::IsInverted == mdrivlib::ST77XX::Inverted ? INVON : INVOFF, .num_args = 0, .delay_ms = 0},

		{.cmd = CASET, .num_args = 4, .delay_ms = 0, .args = {0, ConfT::viewWidth >> 8, 0, ConfT::viewWidth & 0xFF}},
		{.cmd = RASET, .num_args = 4, .delay_ms = 0, .args = {0, ConfT::viewHeight >> 8, 0, ConfT::viewHeight & 0xFF}},

		//// seq 2
		//Enable Table 2 Commands:
		{.cmd = CMD2EN, .num_args = 4, .delay_ms = 1, .args = {0x5A, 0x69, 0x02, 0x01}},

		// RGB Control: V/H back porch
		{.cmd = RGBCTRL,
		 .num_args = 3,
		 .delay_ms = 0,
		 .args = {RGBCTRL_DE_Mode | RGBCTRL_UseRAM, ConfT::VBackPorch, ConfT::HBackPorch /*+ ConfT::HSyncWidth*/}},

		// RAM Control: set RGB mode. EPF and MTD don't matter
		{.cmd = RAMCTRL, .num_args = 2, .delay_ms = 0, .args = {RAMCTRL_RGB_IF, RAMCTRL_EPF_DEFAULT}},

		{.cmd = FRCTRL2, .num_args = 1, .delay_ms = 10, .args = {0x0F}}, //0x0F = 60Hz, 0x0E = 62Hz

		//{.cmd = GCTRL, .num_args = 4, .delay_ms = 10, .args = {0x2A, 0x2B, 0x22 + 6, 0x75}},

		//{.cmd = CABCCTRL, .num_args = 1, .delay_ms = 10, .args = {0b0010}}, //LED no reversed, init state LEDPWM low, LEDPWM fixed to on, LEDPWM Polarity 0/high

		//{.cmd = PWMFRSEL, .num_args = 1, .delay_ms = 10, .args = {(0x04 << 3) | 0x00}}, //CS = 4, CLK = 0 => 666kHz PWM

		// Porch setting: Not sure if this wants V or H porch?
		// {.cmd = PORCTRL, .num_args = 3, .delay_ms = 0, .args = {ConfT::HBackPorch, ConfT::HFrontPorch, 0}},

		//Max brightness
		// {.cmd = WRDISBV, .num_args = 4, .delay_ms = 0, .args = {0xFF,0,0,0xFF}},

		// Normal display on
		// {.cmd = NORON, .num_args = 0, .delay_ms = 10},

		{.cmd = DISPON, .num_args = 0, .delay_ms = 100},
		{.cmd = SLPOUT, .num_args = 0, .delay_ms = 100},
		{.cmd = RAMWR, .num_args = 0, .delay_ms = 50},
	};
};

} // namespace mdrivlib::ST77XX
