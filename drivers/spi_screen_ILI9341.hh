#pragma once
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

struct InitCommand {
	ILI9341::Command cmd;
	uint8_t num_args = 0;
	uint16_t delay_ms = 0;
	uint8_t args[8];
	// std::span<const uint8_t> args; // FixMe: std::span<uint8_t> or std::span<const uint8_t> gives error: No matching
	// constructor
};

enum InvertState { NotInverted, Inverted };
// reset high 1ms, low 10ms, high 120ms

template<uint16_t WIDTH, uint16_t HEIGHT, enum InvertState ISINVERTED>
struct ILI9341Init {
	static constexpr InitCommand cmds[] =
		{
			{.cmd = Reset, .num_args = 0, .delay_ms = 5000},
			{.cmd = DisplayOff, .num_args = 0},
			{.cmd = PowerControlA, .num_args = 5, .delay_ms = 0, .args = {0x39, 0x2C, 0, 0x34, 2}},
			{.cmd = PowerControlB, .num_args = 3, .delay_ms = 0, .args = {0, 0xC1, 0x30}},
			///....
			{.cmd = MemoryAccessControl, .num_args = 1, .delay_ms = 0, .args = {0x08}},
			{.cmd = DisplayNonInvert, .num_args = 0, .delay_ms = 0},
			{.cmd = PixelFormat, .num_args = 1, .delay_ms = 0, .args = {0x55}},

			{.cmd = SleepOut, .num_args = 0, .delay_ms = 120},
			{.cmd = DisplayOn, .num_args = 0},
		};

	static constexpr uint32_t num_commands = sizeof(cmds) / sizeof(InitCommand);
};

} // namespace mdrivlib::ILI9341
