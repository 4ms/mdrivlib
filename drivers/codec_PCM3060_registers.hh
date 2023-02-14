#pragma once

#include "drivers/bus_register.hh"

namespace mdrivlib::CodecPCM3060Register
{
using BusReg::Bits;
using BusReg::ReadWrite;

// Un-shifted address: 0x46 (will send 0x8C for write, 0x8D for read)
constexpr inline uint8_t I2C_BASE_ADDR = 0b1000110;

struct SystemControl : ReadWrite {
	enum : uint8_t { Address = 64 };

	uint8_t DACSingleEnded : 1;
	uint8_t : 3;
	uint8_t DACPowerSave : 1;
	uint8_t ADCPowerSave : 1;
	uint8_t SystemReset : 1;
	uint8_t ModeRegisterReset : 1;

	constexpr operator uint8_t() {
		return (DACSingleEnded << 0) | (DACPowerSave << 4) | (ADCPowerSave << 5) | (SystemReset << 6) |
			   (ModeRegisterReset << 7);
	}
	constexpr static SystemControl make(uint8_t raw) {
		return {
			.DACSingleEnded = Bits<0>(raw),
			.DACPowerSave = Bits<4>(raw),
			.ADCPowerSave = Bits<5>(raw),
			.SystemReset = Bits<6>(raw),
			.ModeRegisterReset = Bits<7>(raw),
		};
	}
};

struct DacControl1 : ReadWrite {
	enum : uint8_t { Address = 67 };

	uint8_t Format : 2;
	uint8_t : 2;
	uint8_t MSInterface : 3;
	uint8_t ClockSel : 1;

	enum Formats {
		I2S_24bit = 0b00,
		LeftJust_24bit = 0b01,
		RightJust_24bit = 0b10,
		RightJust_16bit = 0b11,
	};

	enum Interfaces {
		Slave = 0b000,
		Master768fs = 0b001,
		Master512fs = 0b010,
		Master384fs = 0b011,
		Master256fs = 0b100,
		Master192fs = 0b101,
		Master128fs = 0b110,
	};

	enum ClockSels {
		SelectPinGroups2 = 0,
		SelectPinGroups1 = 1,
	};

	constexpr operator uint8_t() {
		return (Format << 0) | (MSInterface << 4) | (ClockSel << 7);
	}
	constexpr static DacControl1 make(uint8_t raw) {
		return {
			.Format = Bits<0, 1>(raw),
			.MSInterface = Bits<4, 5, 6>(raw),
			.ClockSel = Bits<7>(raw),
		};
	}
};

struct AdcControl1 : ReadWrite {
	enum : uint8_t { Address = 72 };

	uint8_t Format : 2;
	uint8_t : 2;
	uint8_t MSInterface : 3;
	uint8_t ClockSel : 1;

	enum Formats {
		I2S_24bit = 0b00,
		LeftJust_24bit = 0b01,
		RightJust_24bit = 0b10,
		RightJust_16bit = 0b11,
	};

	enum Interfaces {
		Slave = 0b000,
		Master768fs = 0b001,
		Master512fs = 0b010,
		Master384fs = 0b011,
		Master256fs = 0b100,
	};

	enum ClockSels {
		SelectPinGroups1 = 0,
		SelectPinGroups2 = 1,
	};

	constexpr operator uint8_t() {
		return (Format << 0) | (MSInterface << 4) | (ClockSel << 7);
	}
	constexpr static AdcControl1 make(uint8_t raw) {
		return {
			.Format = Bits<0, 1>(raw),
			.MSInterface = Bits<4, 5, 6>(raw),
			.ClockSel = Bits<7>(raw),
		};
	}
};

} // namespace mdrivlib::CodecPCM3060Register
