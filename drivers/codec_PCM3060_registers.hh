#pragma once
#include <variant>

#include "drivers/bus_register.hh"

namespace mdrivlib::CodecPCM3060Register
{
using BusReg::Bits;
using BusReg::ReadWrite;

// Un-shifted address: 0x46
// Shifted: 0x8C for write, 0x8D for read
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

struct DacLeftLevel : ReadWrite {
	enum : uint8_t { Address = 65 };

	uint8_t atten;
	enum AttenAmt {
		_0dB = 255,
		_minus1dB = 253,
		_minus2dB = 251,
		//... etc
		_minus100dB = 55,
		_Mute = 54,
	};

	constexpr operator uint8_t() {
		return atten;
	}
	constexpr static DacLeftLevel make(uint8_t raw) {
		return {.atten = raw};
	}
};

struct DacRightLevel : ReadWrite {
	enum : uint8_t { Address = 66 };

	uint8_t atten;
	enum AttenAmt {
		_0dB = 255,
		_minus1dB = 253,
		_minus2dB = 251,
		//... etc
		_minus100dB = 55,
		_Mute = 54,
	};

	constexpr operator uint8_t() {
		return atten;
	}
	constexpr static DacRightLevel make(uint8_t raw) {
		return {.atten = raw};
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

struct DacControl2 : ReadWrite {
	enum : uint8_t { Address = 68 };
	//TODO
	uint8_t raw;

	constexpr operator uint8_t() {
		return raw;
	}
	constexpr static DacControl2 make(uint8_t raw) {
		return {.raw = raw};
	}
};

struct DacControl3 : ReadWrite {
	enum : uint8_t { Address = 69 };
	//TODO
	uint8_t raw;

	constexpr operator uint8_t() {
		return raw;
	}
	constexpr static DacControl3 make(uint8_t raw) {
		return {.raw = raw};
	}
};

struct AdcLeftLevel : ReadWrite {
	enum : uint8_t { Address = 70 };

	uint8_t atten;
	enum AttenAmt {
		_plus20dB = 255,
		_plus19dB = 253,

		_0dB = 215,

		_minus100dB = 15,
		_Mute = 14,
	};

	constexpr operator uint8_t() {
		return atten;
	}
	constexpr static AdcLeftLevel make(uint8_t raw) {
		return {.atten = raw};
	}
};

struct AdcRightLevel : ReadWrite {
	enum : uint8_t { Address = 71 };

	uint8_t atten;
	enum AttenAmt {
		_plus20dB = 255,
		_plus19dB = 253,

		_0dB = 215,

		_minus100dB = 15,
		_Mute = 14,
	};

	constexpr operator uint8_t() {
		return atten;
	}
	constexpr static AdcRightLevel make(uint8_t raw) {
		return {.atten = raw};
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

struct AdcControl2 : ReadWrite {
	enum : uint8_t { Address = 73 };

	uint8_t SoftMuteEnableLeft : 1;
	uint8_t SoftMuteEnableRight : 1;
	uint8_t InvertPhase : 1;
	uint8_t HPFBypass : 1;
	uint8_t ZeroCrossDetectDisable : 1;
	uint8_t : 3;

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
		return (SoftMuteEnableLeft << 0) | (SoftMuteEnableLeft << 1) | (InvertPhase << 2) | (HPFBypass << 3) |
			   (ZeroCrossDetectDisable << 4);
	}
	constexpr static AdcControl2 make(uint8_t raw) {
		return {
			.SoftMuteEnableLeft = Bits<0>(raw),
			.SoftMuteEnableRight = Bits<1>(raw),
			.InvertPhase = Bits<2>(raw),
			.HPFBypass = Bits<3>(raw),
			.ZeroCrossDetectDisable = Bits<4>(raw),
		};
	}
};

using AnyRegister = std::variant<SystemControl,
								 DacLeftLevel,
								 DacRightLevel,
								 DacControl1,
								 DacControl2,
								 DacControl3,
								 AdcLeftLevel,
								 AdcRightLevel,
								 AdcControl1,
								 AdcControl2>;

} // namespace mdrivlib::CodecPCM3060Register
