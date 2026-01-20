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

struct Page : ReadWrite {
	static constexpr uint8_t Address = 0x00;

	uint8_t page;
};

struct SwReset : ReadWrite {
	static constexpr uint8_t Address = 0x01;

	uint8_t Reset : 1;
	uint8_t : 7;
};

using Registers = std::variant<Page, SwReset>;

} // namespace mdrivlib::CodecTLV320AIC3204Register
