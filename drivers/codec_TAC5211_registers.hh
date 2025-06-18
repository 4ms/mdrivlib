#pragma once
#include <cstdint>

namespace mdrivlib::CodecTAC5211Register
{
constexpr uint8_t I2C_BASE_ADDR = 0xA0; // Shifted address. See Datasheet section 7.5.1

// The procedure for register access across pages is:
// • Select page N (write data N to register 0 regardless of the current page number)
// • Read or write data from or to valid registers in page N
// • Select the new page M (write data M to register 0 regardless of the current page number)
// • Read or write data from or to valid registers in page M
// • Repeat as needed

struct Page {
	static constexpr uint8_t Address = 0x00;
	enum PAGE_CFG : uint8_t { Page0 = 0, Page1 = 1, Page3 = 3 };
};

struct SwReset {
	static constexpr uint8_t Address = 0x01;
	enum SW_RESET : uint8_t { Reset = (1 << 0) };
};

struct Config0 {
	static constexpr uint8_t Address = 0x1A;
	enum Format : uint8_t { TDM = 0, I2S = 1, LeftJust = 2 };
	enum WLen : uint8_t { Bits16 = 0, Bits24 = 1, Bits32 = 3 };
	enum FsyncPolarity : uint8_t { Standard = 0, Inverted = 1 };
	enum BitClkPolarity : uint8_t { BClkStandard = 0, BClkInverted = 1 };
	enum BusErr : uint8_t { DetectBusErr = 0, DontDetectBusErr = 1 };
	enum BusErrRecover : uint8_t { ResumeAfterBusErr = 0, DontResumeAfterBusErr = 1 };
};

} // namespace mdrivlib::CodecTAC5211Register
