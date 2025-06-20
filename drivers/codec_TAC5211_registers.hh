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

using BusReg::Bits;
using BusReg::ReadClear;
using BusReg::ReadOnly;
using BusReg::ReadWrite;
using BusReg::WriteClear;

struct Page : ReadWrite {
	static constexpr uint8_t Address = 0x00;

	uint8_t page;
};

struct SwReset : ReadWrite {
	static constexpr uint8_t Address = 0x01;

	uint8_t Reset : 1;
	uint8_t : 7;
};

struct DeviceMiscConfig : ReadWrite {
	static constexpr uint8_t Address = 0x02;

	uint8_t WakeFromSleep : 1;
	uint8_t IOvddIs1_8V : 1;
	uint8_t AvddIs1_8V : 1;
	uint8_t SleepExitEnableVref : 1;

	enum class VrefQuickCharges : uint8_t {
		Duration_3_5ms = 0,
		Duration_10ms = 1,
		Duration_50ms = 2,
		Duration_100ms = 3
	};
	VrefQuickCharges VrefQuickCharge : 2;

	uint8_t : 2;
};

struct IntFConfig1 : ReadWrite {
	static constexpr uint8_t Address = 0x10;

	uint8_t DoutDrive : 3;
	uint8_t DoutGPIOVal : 1;
	uint8_t DoutSel : 4;
};

struct Config0 : ReadWrite {
	static constexpr uint8_t Address = 0x1A;

	enum class BusErrorRecoveries : uint8_t { ResumeAfterBusErr = 0, DontResumeAfterBusErr = 1 };
	BusErrorRecoveries BusErrorRecovery : 1;

	enum class BusErr : uint8_t { DetectBusErr = 0, DontDetectBusErr = 1 };
	BusErr BusErrorDetection : 1;

	enum class Polarity : uint8_t { Standard = 0, Inverted = 1 };
	Polarity BitClkPolarity : 1;
	Polarity FsyncPolarity : 1;

	enum class Length : uint8_t { Bits16 = 0, Bits24 = 1, Bits32 = 3 };
	Length SlotLength : 2;

	enum class Formats : uint8_t { TDM = 0, I2S = 1, LeftJust = 2 };
	Formats Format : 2;
};

using enum Config0::BusErrorRecoveries;
using enum Config0::BusErr;
using enum Config0::Polarity;
using enum Config0::Length;
using enum Config0::Formats;

// P0_R30: 0x1E
// P0_R37: 0x25
// P0_R40: 0x28
// P0_R47: 0x2F

struct ClockConfig2 : ReadWrite {
	static constexpr uint8_t Address = 0x34;

	uint8_t RatioClockEdge : 1;
	enum RatioClockEdges { Rising = 0, Falling = 1 };

	uint8_t ClockSource : 3;
	enum ClockSources { BCLK = 0, CCLK_FSync = 1, BCLK2 = 2, CCLK_FSync2 = 3, CCLK_Fixed = 4, Internal = 5 };

	uint8_t : 2;
	uint8_t AllowFractionalPLL : 1;
	uint8_t DisablePLL : 1;
};

struct VrefMicBiasConfig : ReadWrite {
	static constexpr uint8_t Address = 0x4d;

	uint8_t VrefFullScale : 2;
	enum VrefFullScales { Vref2750mV = 0, Vref2500mV = 1, Vref1375mV = 2 };

	uint8_t MicBias : 2;
	enum MicBiases { BiasVref = 0, BiasVref_div2 = 1, BiasAvdd = 3 };

	uint8_t MicBiasLdoGain1_096 : 1;
	uint8_t : 3;
};

struct AdcChan1Config : ReadWrite {
	static constexpr uint8_t Address = 0x50;

	uint8_t Bandwidth : 1;
	enum Bandwidths { Audio24kHz = 0, Wide96kHz = 1 };

	uint8_t FullScaleValue : 1;
	enum FullScaleValues { Diff2Vrms = 0, Diff4Vrms = 1 };

	uint8_t CommonModeTolerance : 2;
	enum CommonModeTolerances { Low50mV = 0, Med500mV = 1, HighR2R = 2 };

	uint8_t Impedance : 2;
	enum Impendances { Imp5k = 0, Imp10k = 1, Imp40k = 2 };

	uint8_t InSource : 2;
	enum InSources { Diff = 0, Single = 1, SingleInP1 = 2, SingleInM1 = 3 };
};

struct Out1Config0 : ReadWrite {
	static constexpr uint8_t Address = 0x64;

	uint8_t : 1;

	uint8_t Vcom : 1;
	enum Vcoms { Vref_div_0_6 = 0, Avdd_div_2 = 1 };

	uint8_t RoutingConfig : 3;
	enum RoutingConfigs {
		Diff = 0,
		Stereo = 1,
		MonoOut1P = 2,
		MonoOut1M = 3,
		PseudoDiffOut1P = 4,
		PseudoDiffOut1M = 6
	};

	uint8_t OutSource : 2;
	enum OutSources { DAC = 1, Bypass = 2, DACandBypass = 3, DACOut1P_BypassOut1M = 4, DACOut1M_BypassOut1P = 5 };
};

struct Out1Config1 : ReadWrite {
	static constexpr uint8_t Address = 0x65;

	uint8_t Bandwidth : 1;
	enum Bandwidths { Audio24kHz = 0, Wide96kHz = 1 };

	uint8_t BypassIN1Config : 1;
	enum BypassInputConfigs { Diff = 0, Single = 1 };

	uint8_t AIn1MBypassImpedance : 1;
	enum BypassImpedances { Imp4400ohm = 0, Imp20kohm = 1 };

	uint8_t LevelOut1P : 3;
	enum Levels { dB12 = 2, dB6 = 3, dB0 = 4, dBn6 = 5, dBn12 = 6 };

	uint8_t DriveOut1P : 2;
	enum Drives { LineOut300 = 0, Headphone16 = 1, Low4ohm = 2, FDreceiverLoad = 3 };
};

struct Out1Config2 : ReadWrite {
	static constexpr uint8_t Address = 0x66;

	uint8_t CommonModeTolerance : 1;
	enum CommonModeTolerances { Tol50mV = 0, TolR2R = 1 };

	uint8_t : 1;

	uint8_t AIn1PBypassImpedance : 1;
	enum BypassImpedances { Imp4400ohm = 0, Imp20kohm = 1 };

	uint8_t LevelOut1M : 3;
	enum Levels { dB12 = 2, dB6 = 3, dB0 = 4, dBn6 = 5, dBn12 = 6 };

	uint8_t DriveOut1M : 2;
	enum Drives { LineOut300 = 0, Headphone16 = 1, Low4ohm = 2, FDreceiverLoad = 3 };
};

struct ChannelEnable : ReadWrite {
	static constexpr uint8_t Address = 0x76;

	uint8_t OutChan4Enable : 1;
	uint8_t OutChan3Enable : 1;
	uint8_t OutChan2Enable : 1;
	uint8_t OutChan1Enable : 1;
	uint8_t InChan4Enable : 1;
	uint8_t InChan3Enable : 1;
	uint8_t InChan2Enable : 1;
	uint8_t InChan1Enable : 1;
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
};

using Registers = std::variant<Page,
							   SwReset,
							   DeviceMiscConfig,
							   IntFConfig1,
							   Config0,
							   ClockConfig2,
							   VrefMicBiasConfig,
							   AdcChan1Config,
							   Out1Config0,
							   Out1Config1,
							   Out1Config2,
							   ChannelEnable,
							   PowerConfig>;

} // namespace mdrivlib::CodecTAC5211Register
