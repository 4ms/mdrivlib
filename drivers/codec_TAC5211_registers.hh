#pragma once
#include "drivers/bus_register.hh"
#include <cstdint>
#include <variant>

namespace mdrivlib::CodecTAC5211Register
{
constexpr uint8_t I2C_BASE_ADDR = 0xA0; // Shifted address. See Datasheet section 7.5.1

// The procedure for register access across pages is:
// • Select page N (write data N to register 0 regardless of the current page number)
// • Read or write data from or to valid registers in page N
// • Select the new page M (write data M to register 0 regardless of the current page number)
// • Read or write data from or to valid registers in page M
// • Repeat as needed

// struct Page {
// 	static constexpr uint8_t Address = 0x00;
// 	enum PAGE_CFG : uint8_t { Page0 = 0, Page1 = 1, Page3 = 3 };
// };

// struct SwReset {
// 	static constexpr uint8_t Address = 0x01;
// 	enum SW_RESET : uint8_t { Reset = (1 << 0) };
// };

// struct Config0 {
// 	static constexpr uint8_t Address = 0x1A;
// 	enum Format : uint8_t { TDM = 0, I2S = 1, LeftJust = 2 };
// 	enum WLen : uint8_t { Bits16 = 0, Bits24 = 1, Bits32 = 3 };
// 	enum FsyncPolarity : uint8_t { Standard = 0, Inverted = 1 };
// 	enum BitClkPolarity : uint8_t { BClkStandard = 0, BClkInverted = 1 };
// 	enum BusErr : uint8_t { DetectBusErr = 0, DontDetectBusErr = 1 };
// 	enum BusErrRecover : uint8_t { ResumeAfterBusErr = 0, DontResumeAfterBusErr = 1 };
// };

/////////////////

using BusReg::Bits;
using BusReg::ReadClear;
using BusReg::ReadOnly;
using BusReg::ReadWrite;
using BusReg::WriteClear;

struct Page : ReadWrite {
	static constexpr uint8_t Address = 0x00;
	uint8_t page;

	constexpr operator uint8_t() {
		return page;
	}
};

struct SwReset : ReadWrite {
	static constexpr uint8_t Address = 0x01;
	uint8_t Reset : 1;
	uint8_t : 7;

	constexpr operator uint8_t() {
		return Reset;
	}
};

struct DeviceMiscConfig : ReadWrite {
	static constexpr uint8_t Address = 0x02;
	uint8_t WakeFromSleep : 1;

	uint8_t IOvddIs1_8V : 1;

	uint8_t AvddIs1_8V : 1;

	uint8_t SleepExitEnableVref : 1;

	uint8_t VrefQuickCharge : 2;
	enum VrefQuickCharges { Duration_3_5ms = 0, Duration_10ms = 1, Duration_50ms = 2, Duration_100ms = 3 };

	uint8_t : 2;

	constexpr operator uint8_t() {
		return (WakeFromSleep << 0) | (IOvddIs1_8V << 1) | (AvddIs1_8V << 2) | (SleepExitEnableVref << 3) |
			   (VrefQuickCharge << 4);
		;
	}
};

struct IntFConfig1 : ReadWrite {
	static constexpr uint8_t Address = 0x10;

	uint8_t DoutDrive : 3;
	uint8_t DoutGPIOVal : 1;
	uint8_t DoutSel : 4;

	constexpr operator uint8_t() {
		return (DoutDrive << 0) | (DoutGPIOVal << 3) | (DoutSel << 4);
		;
	}
};

struct Config0 : ReadWrite {
	static constexpr uint8_t Address = 0x1A;

	uint8_t BusErrorRecovery : 1;
	enum BusErrRecover : uint8_t { ResumeAfterBusErr = 0, DontResumeAfterBusErr = 1 };

	uint8_t BusErrorDetection : 1;
	enum BusErr : uint8_t { DetectBusErr = 0, DontDetectBusErr = 1 };

	uint8_t BitClkPolarity : 1;
	uint8_t FsyncPolarity : 1;
	enum Polarity : uint8_t { Standard = 0, Inverted = 1 };

	uint8_t SlotLength : 2;
	enum WLen : uint8_t { Bits16 = 0, Bits24 = 1, Bits32 = 3 };

	uint8_t Format : 2;
	enum Format : uint8_t { TDM = 0, I2S = 1, LeftJust = 2 };

	constexpr operator uint8_t() {
		return (BusErrorRecovery << 0) | (BusErrorDetection << 1) | (BitClkPolarity << 2) | (FsyncPolarity << 3) |
			   (SlotLength << 4) | (Format << 6);
		;
	}
};
static_assert((uint8_t)Config0{.SlotLength = Config0::Bits24, .Format = Config0::I2S} == 0x50);

struct ClockConfig2 : ReadWrite {
	static constexpr uint8_t Address = 0x34;

	uint8_t RatioClockEdge : 1;
	enum RatioClockEdges { Rising = 0, Falling = 1 };

	uint8_t ClockSource : 3;
	enum ClockSources { BCLK = 0, CCLK_FSync = 1, BCLK2 = 2, CCLK_FSync2 = 3, CCLK_Fixed = 4, Internal = 5 };

	uint8_t : 2;
	uint8_t AllowFractionalPLL : 1;
	uint8_t DisablePLL : 1;

	constexpr operator uint8_t() {
		return (RatioClockEdge << 0) | (ClockSource << 1) | (AllowFractionalPLL << 6) | (DisablePLL << 7);
	}
};

struct PowerConfig : ReadWrite {
	static constexpr uint8_t Address = 0x78;

	uint8_t : 1;
	uint8_t UagEnable : 1;
	uint8_t VoiceActivityEnable : 1;
	uint8_t UadEnable : 1;
	uint8_t : 1;
	uint8_t MicBiasPowered : 1;
	uint8_t DacPowered : 1;
	uint8_t AdcPowered : 1;

	constexpr operator uint8_t() {
		return (AdcPowered << 0) | (DacPowered << 1) | (MicBiasPowered << 2) | (UadEnable << 4) |
			   (VoiceActivityEnable << 5) | (UagEnable << 6);
		;
	}
};

using Registers = std::variant<Page, SwReset, DeviceMiscConfig, IntFConfig1, Config0, ClockConfig2, PowerConfig>;

} // namespace mdrivlib::CodecTAC5211Register
