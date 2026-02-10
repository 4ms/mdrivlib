#pragma once
#include <array>
#include <variant>

#include "drivers/bus_register.hh"

namespace mdrivlib::CodecTLV320AIC3204Register
{
using BusReg::Bits;
using BusReg::ReadClear;
using BusReg::ReadOnly;
using BusReg::ReadWrite;
using BusReg::WriteClear;

// Datasheet:
// https://www.ti.com/lit/ds/symlink/tlv320aic3204.pdf
// Application reference:
// https://www.ti.com/lit/ml/slaa557/slaa557.pdf

// Un-shifted address: 0x18
// Shifted: 0x30 for write, 0x31 for read
// Datasheet section 9.3.8.1
constexpr inline uint8_t I2C_BASE_ADDR = 0b0011000;

struct PageSelect : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x00;

	uint8_t page;
};

struct SwReset : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x01;

	uint8_t Reset : 1;
	uint8_t : 7;
};

struct ClockSettingMultiplexors : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x04;

	uint8_t CodecClkIn : 2;
	enum CodecClkIns { MCLK = 0b00, BCLK = 0b01, GPIO = 0b10, PLL = 0b11 };

	uint8_t PLLInputClock : 2;
	enum PLLInputClocks { PLL_MCLK = 0b00, PLL_BCLK = 0b01, PLL_GPIO = 0b10, PLL_DIN = 0b11 };

	uint8_t : 2;

	uint8_t HighPLLClockRange : 1;

	uint8_t : 1;
};

struct ClockSettingPLLPandRValues : ReadWrite {
	static constexpr uint8_t Page = 0x00;
	static constexpr uint8_t Address = 0x05;

	uint8_t PLLDividerR : 3;
	enum PLLDividerRs { R1 = 0b001, R2, R3, R4 };

	uint8_t PLLDividerP : 3;
	enum PLLDividerPs { P8 = 0b000, P1 = 0b001, P2, P3, P4, P5, P6, P7 };

	uint8_t PLLPowerUp : 1;
};

using Registers = std::variant<PageSelect, SwReset, ClockSettingMultiplexors, ClockSettingPLLPandRValues>;

} // namespace mdrivlib::CodecTLV320AIC3204Register
