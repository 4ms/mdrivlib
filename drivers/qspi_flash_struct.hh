#pragma once
#include "pin.hh"

namespace mdrivlib
{
struct QSPIFlashConfig {
	PinNoInit io0{};
	PinNoInit io1{};
	PinNoInit io2{};
	PinNoInit io3{};
	PinNoInit clk{};
	PinNoInit cs{};

	uint32_t clock_division = 4;
	uint32_t IRQ_pri = 2;
	uint32_t IRQ_subpri = 2;

	uint32_t flash_size_bytes = 0x40000;

	enum AddressBits { AddrBits24 = 24, AddrBits32 = 32 };
	AddressBits flash_size_address_bits = AddrBits24;

	enum ChipID { IS25L, S25FLxxxL };
	ChipID chip_id = IS25L;

	enum IOMode { SDR, DDR, QPI };
	IOMode io_mode = QPI;
};

//TODO: constexpr struct for config
// struct QSPIFlashConfig {
// 	static constexpr PinNoInit io0{};
// 	static constexpr PinNoInit io1{};
// 	static constexpr PinNoInit io2{};
// 	static constexpr PinNoInit io3{};
// 	static constexpr PinNoInit clk{};
// 	static constexpr PinNoInit cs{};

// 	static constexpr uint32_t clock_division = 4;
// 	static constexpr uint32_t IRQ_pri = 2;
// 	static constexpr uint32_t IRQ_subpri = 2;

// 	static constexpr uint32_t flash_size_bytes = 0x40000;
// 	enum AddressBits { AddrBits24 = 24, AddrBits32 = 32 };
// 	static constexpr AddressBits flash_size_address_bits = AddrBits24;

// 	enum IOMode { SDR, DDR, QPI };
// 	static constexpr IOMode io_mode = QPI;
// };
} // namespace mdrivlib
