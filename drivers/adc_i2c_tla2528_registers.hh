#pragma once
#include "drivers/bus_register.hh"
#include <cstdint>

namespace mdrivlib::TLA2528
{
using BusReg::Bits;
using BusReg::ReadClear;
using BusReg::ReadOnly;
using BusReg::ReadWrite;
using BusReg::WriteClear;

struct SystemStatus : ReadWrite {
	enum : uint8_t { Address = 0x00 };

	uint8_t : 7;
	uint8_t RsvdAlwaysHigh : 1;

	constexpr static SystemStatus make(uint8_t raw) {
		return {
			.RsvdAlwaysHigh = Bits<7>(raw),
		};
	}
};

struct GeneralCfg : ReadWrite {
	enum : uint8_t { Address = 0x01 };

	uint8_t Reset : 1; //W only
	uint8_t Calibrate : 1;
	uint8_t ForceChannelsAnalog : 1;
	uint8_t StartConversion : 1; //W only
	uint8_t : 4;

	constexpr static GeneralCfg make(uint8_t raw) {
		return {.Reset = Bits<0>(raw),
				.Calibrate = Bits<1>(raw),
				.ForceChannelsAnalog = Bits<2>(raw),
				.StartConversion = Bits<3>(raw)};
	}
};

struct DataCfg : ReadWrite {
	enum : uint8_t { Address = 0x02 };

	uint8_t : 4;
	uint8_t AppendStatus : 2;
	uint8_t : 1;
	uint8_t DebugOutput0xA5A : 1;

	constexpr static DataCfg make(uint8_t raw) {
		return {.AppendStatus = Bits<4, 5>(raw), .DebugOutput0xA5A = Bits<7>(raw)};
	}
};

struct OversampleCfg : ReadWrite {
	enum : uint8_t { Address = 0x03 };

	uint8_t OversampleBits : 3;
	uint8_t : 5;

	constexpr static OversampleCfg make(uint8_t raw) {
		return {
			.OversampleBits = Bits<0, 1, 2>(raw),
		};
	}
};

struct OpMode : ReadWrite {
	enum : uint8_t { Address = 0x04 };

	uint8_t ClockDiv : 4;
	uint8_t OscSel : 1;
	uint8_t : 3;

	enum OscSels { OscSelLowPower = 1, OscSelHighSpeed = 0 };

	constexpr static OpMode make(uint8_t raw) {
		return {
			.ClockDiv = Bits<0, 1, 2, 3>(raw),
			.OscSel = Bits<4>(raw),
		};
	}
};

struct PinCfg : ReadWrite {
	enum : uint8_t { Address = 0x05 };

	uint8_t BitMaskDigitalGPIO; //bitmask 1 = GPIO, 0 = analog

	constexpr static PinCfg make(uint8_t raw) {
		return PinCfg{.BitMaskDigitalGPIO = raw};
	}
};

struct GPIOCfg : WriteClear {
	enum : uint8_t { Address = 0x07 };

	uint8_t BitMaskOutputs; //bitmask 1 = output, 0 = input

	constexpr static GPIOCfg make(uint8_t raw) {
		return GPIOCfg{.BitMaskOutputs = raw};
	}
};

struct GPOCfg : ReadWrite {
	enum : uint8_t { Address = 0x09 };

	uint8_t BitMaskOutputsPushPull; //bitmask: 1 = push-pull, 0 = open-drain

	constexpr static GPOCfg make(uint8_t raw) {
		return {.BitMaskOutputsPushPull = raw};
	}
};

struct GPOValue : ReadWrite {
	enum : uint8_t { Address = 0x0b };

	uint8_t BitMaskSetOutput;

	constexpr static GPOValue make(uint8_t raw) {
		return {.BitMaskSetOutput = raw};
	}
};

struct GPIValue : ReadOnly {
	enum : uint8_t { Address = 0x0d };

	uint8_t BitMaskReadInput;

	constexpr static GPIValue make(uint8_t raw) {
		return {.BitMaskReadInput = raw};
	}
};

struct SequenceCfg : ReadWrite {
	enum : uint8_t { Address = 0x10 };

	uint8_t SeqModeAuto : 1;
	uint8_t SeqModeReserved : 1;
	uint8_t : 2;
	uint8_t SeqStart : 1;
	uint8_t : 3;

	constexpr static SequenceCfg make(uint8_t raw) {
		return {
			.SeqModeAuto = Bits<0>(raw),
			.SeqModeReserved = Bits<1>(raw),
			.SeqStart = Bits<4>(raw),
		};
	}
};

struct ChannelSel : ReadWrite {
	enum : uint8_t { Address = 0x11 };

	uint8_t ManualChannelSel : 3;
	uint8_t : 1;
	uint8_t : 4;

	constexpr static ChannelSel make(uint8_t raw) {
		return {
			.ManualChannelSel = Bits<0, 1, 2>(raw),
		};
	}
};

struct AutoSeqChanSel : WriteClear {
	enum : uint8_t { Address = 0x12 };

	uint8_t BitMaskChannelEnabled;

	constexpr static AutoSeqChanSel make(uint8_t raw) {
		return {.BitMaskChannelEnabled = raw};
	}
};

} // namespace mdrivlib::TLA2528

// How this was created:
// First, manually created the bitfield structs.
//
// Create operator uint8_t() for bitfields with all 1-bit fields:
//%s/^struct .*\zs\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;/\0\r\toperator uint8_t() { return (\1 << 0) | (\2 << 1) | (\3 << 2) | (\4 << 3) | (\5 << 4) | (\6 << 5) | (\7 << 6) | (\8 << 7); }/
// Create operator uint8_t for bitfields with seven fields. Must fill in shifts manually
// s/^struct .*\zs\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\ze};/\0\r\toperator uint8_t() { return (\1 << 0) | (\2 << X) | (\3 << X) | (\4 << X) | (\5 << X) | (\6 << X) | (\7 << X); }/
// Create operator uint8_t for bitfields with six fields. Must fill in shifts manually
// s/^struct .*\zs\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\ze};/\0\r\toperator uint8_t() { return (\1 << 0) | (\2 << X) | (\3 << X) | (\4 << X) | (\5 << X) | (\6 << X); }/
// Create operator uint8_t for bitfields with five fields. Must fill in shifts manually
// s/^struct .*\zs\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\ze};/\0\r\toperator uint8_t() { return (\1 << 0) | (\2 << X) | (\3 << X) | (\4 << X) | (\5 << X); }/
// Create operator uint8_t for bitfields with four fields. Must fill in shifts manually
// s/^struct .*\zs\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\tuint8_t \(.*\) : \d;\n\ze};/\0\r\toperator uint8_t() { return (\1 << 0) | (\2 << X) | (\3 << X) | (\4 << X); }/
// Three, two, and one fields are easy to do manually (typically have one or more _res)
// Afterwards, remove all (<< \d);
// %s/ | (<< \d)//g
