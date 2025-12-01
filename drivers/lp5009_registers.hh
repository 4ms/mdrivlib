#pragma once
#include "drivers/bus_register.hh"
#include <cstdint>

// Also LP5012 where indicated with comments
namespace mdrivlib::LP5009
{
using BusReg::Bits;
using BusReg::ReadClear;
using BusReg::ReadOnly;
using BusReg::ReadWrite;
using BusReg::WriteClear;

struct DeviceConfig0 : ReadWrite {
	enum : uint8_t { Address = 0x00 };

	uint8_t : 6;
	uint8_t ChipEnable : 1;
	uint8_t : 1;

	constexpr operator uint8_t() {
		return (ChipEnable << 6);
	}
};

struct DeviceConfig1 : ReadWrite {
	enum : uint8_t { Address = 0x01 };

	uint8_t GlobalOff : 1;
	uint8_t MaxCurrentOption : 1;
	uint8_t PWMDitheringEnable : 1;
	uint8_t AutoIncEnable : 1;
	uint8_t PowerSaveEnable : 1;
	uint8_t LogScaleEnable : 1;
	uint8_t : 2;

	constexpr operator uint8_t() {
		return (GlobalOff << 0) | (MaxCurrentOption << 1) | (PWMDitheringEnable << 2) | (AutoIncEnable << 3) |
			   (PowerSaveEnable << 4) | (LogScaleEnable << 5);
	}
};

struct LEDConfig : ReadWrite {
	enum : uint8_t { Address = 0x02 };

	uint8_t Led0BankEnable : 1;
	uint8_t Led1BankEnable : 1;
	uint8_t Led2BankEnable : 1;
	uint8_t Led3BankEnable : 1; // LP5012 only
	uint8_t : 4;
	constexpr operator uint8_t() {
		return (Led0BankEnable << 0) | (Led1BankEnable << 1) | (Led2BankEnable << 2) | (Led3BankEnable << 3);
	}
};

struct BankBrightness : ReadWrite {
	enum : uint8_t { Address = 0x03 };

	uint8_t Brightness;
};

struct BankAColor : ReadWrite {
	enum : uint8_t { Address = 0x04 };

	uint8_t Color;
};

struct BankBColor : ReadWrite {
	enum : uint8_t { Address = 0x05 };

	uint8_t Color;
};

struct BankCColor : ReadWrite {
	enum : uint8_t { Address = 0x06 };

	uint8_t Color;
};

struct LED0Brightness : ReadWrite {
	enum : uint8_t { Address = 0x07 };

	uint8_t Brightness;
};

struct LED1Brightness : ReadWrite {
	enum : uint8_t { Address = 0x08 };

	uint8_t Brightness;
};

struct LED2Brightness : ReadWrite {
	enum : uint8_t { Address = 0x09 };

	uint8_t Brightness;
};

// LP5012 only
struct LED3Brightness : ReadWrite {
	enum : uint8_t { Address = 0x0a };

	uint8_t Brightness;
};

struct Out0Brightness : ReadWrite {
	enum : uint8_t { Address = 0x0a };

	uint8_t Brightness;
};

//... Out1-Out7 are 0x0b to 0x12

struct Out8Brightness : ReadWrite {
	enum : uint8_t { Address = 0x13 };

	uint8_t Brightness;
};

//LP5012 only
struct Out9Brightness : ReadWrite {
	enum : uint8_t { Address = 0x14 };

	uint8_t Brightness;
};
//LP5012 only
struct Out10Brightness : ReadWrite {
	enum : uint8_t { Address = 0x15 };

	uint8_t Brightness;
};
//LP5012 only
struct Out11Brightness : ReadWrite {
	enum : uint8_t { Address = 0x16 };

	uint8_t Brightness;
};

struct Reset : BusReg::WriteOnly {
	enum : uint8_t { Address = 0x17 };

	uint8_t ResetAllRegisters;
	enum { ResetAll = 0xFF };
};

} // namespace mdrivlib::LP5009
