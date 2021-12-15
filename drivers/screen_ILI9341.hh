#pragma once
#include "drivers/screen_ltdc_setup.hh"
#include "util/math.hh"
#include <array>
#include <cstdint>
#include <span>

namespace mdrivlib::ILI9341
{

enum Command : uint8_t {
	Reset = 0x01,
	SleepOut = 0x11,
	DisplayOff = 0x28,
	DisplayOn = 0x29,
	ColSet = 0x2A,
	RowSet = 0x2B,
	ColorMode = 0x3A,
	PowerControlA = 0xCB,
	PowerControlB = 0xCF,
	DriverTimingA = 0xE8,
	DriverTimingB = 0xEA,
	PowerOnSequence = 0xED,
	PumpRatioControl = 0xF7,
	PowerControl1 = 0xC0,
	PowerControl2 = 0xC1,
	VCOMControl1 = 0xC5,
	VCOMControl2 = 0xC7,

	MemoryAccessControl = 0x36,
	DisplayNonInvert = 0x20,
	DisplayInvert = 0x21,
	PixelFormat = 0x3A,

	FrameRateControl = 0xB1,
	DisplayFunctionControl = 0xB6,

};

enum InvertState { NotInverted, Inverted };

// TODO:
// reset high 1ms, low 10ms, high 120ms

struct ILI9341InitSPI {
	//TODO
};

//FIXME: Doesn't work!
// template<uint16_t WIDTH, uint16_t HEIGHT, enum InvertState ISINVERTED>
struct ILI9341InitLTDC {
	static constexpr ScreenInitCommand cmds[] = {
		{.cmd = 0xCF, .num_args = 3, .delay_ms = 0, .args = {0x00, 0xc3, 0x30}},
		{.cmd = 0xED, .num_args = 4, .delay_ms = 0, .args = {0x64, 0x03, 0x12, 0x81}},
		{.cmd = 0xE8, .num_args = 3, .delay_ms = 0, .args = {0x85, 0x10, 0x79}},
		{.cmd = 0xCB, .num_args = 5, .delay_ms = 0, .args = {0x39, 0x2C, 0x00, 0x34, 0x02}},
		{.cmd = 0xF7, .num_args = 1, .delay_ms = 0, .args = {0x20}},
		{.cmd = 0xEA, .num_args = 2, .delay_ms = 0, .args = {0x00, 0x00}},
		//Power control //VRH[5:0]
		{.cmd = 0xC0, .num_args = 1, .delay_ms = 0, .args = {0x22}},
		//Power control //SAP[2:0];BT[3:0]
		{.cmd = 0xC1, .num_args = 1, .delay_ms = 0, .args = {0x11}},
		//VCM control // 3a	46
		{.cmd = 0xC5, .num_args = 2, .delay_ms = 0, .args = {0x3a, 0x1c}},
		//VCM control2 // b0  0xc5	  8b
		{.cmd = 0xC7, .num_args = 1, .delay_ms = 0, .args = {0xa9}},
		// Memory Access Control
		{.cmd = 0x36, .num_args = 1, .delay_ms = 0, .args = {0x08}},
		{.cmd = 0x3A, .num_args = 1, .delay_ms = 0, .args = {0x55}},
		//70hz
		{.cmd = 0xB1, .num_args = 2, .delay_ms = 0, .args = {0x00, 0x1B}},
		// Display Function Control
		{.cmd = 0xB6, .num_args = 2, .delay_ms = 0, .args = {0x0A, 0xA2}},
		{.cmd = 0xF6, .num_args = 2, .delay_ms = 0, .args = {0x01, 0x1d}},
		// 3Gamma Function Disable
		{.cmd = 0xF2, .num_args = 1, .delay_ms = 0, .args = {0x00}},
		//Gamma curve selected
		// {.cmd = 0x26, .num_args = 1, .delay_ms = 0, .args = {0x01}},

		{.cmd = PixelFormat, .num_args = 1, .delay_ms = 10, .args = {0x55}},

		{.cmd = DisplayInvert, .num_args = 0, .delay_ms = 0},
		{.cmd = SleepOut, .num_args = 0, .delay_ms = 120},
		{.cmd = DisplayOn, .num_args = 0, .delay_ms = 50},

		{.cmd = RowSet, .num_args = 4, .delay_ms = 0, .args = {0, 0, (320 >> 8), 320 & 0x00FF}},
		{.cmd = ColSet, .num_args = 4, .delay_ms = 0, .args = {0, 0, 0, 240}},

		//{.cmd = Reset, .num_args = 0, .delay_ms = 5000},
		//{.cmd = DisplayOff, .num_args = 0},
		//{.cmd = PowerControlA, .num_args = 5, .delay_ms = 0, .args = {0x39, 0x2C, 0, 0x34, 2}},
		//{.cmd = PowerControlB, .num_args = 3, .delay_ms = 0, .args = {0, 0xC1, 0x30}},
		/////....
		//{.cmd = MemoryAccessControl, .num_args = 1, .delay_ms = 0, .args = {0x08}},
		//{.cmd = DisplayNonInvert, .num_args = 0, .delay_ms = 0},
		// {.cmd = PixelFormat, .num_args = 1, .delay_ms = 10, .args = {0x55}},

		//{.cmd = SleepOut, .num_args = 0, .delay_ms = 120},
		//{.cmd = DisplayOn, .num_args = 0},
	};

	static constexpr uint32_t num_commands = sizeof(cmds) / sizeof(ScreenInitCommand);
};

} // namespace mdrivlib::ILI9341
