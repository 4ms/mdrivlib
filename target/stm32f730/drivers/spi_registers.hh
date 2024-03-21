#pragma once
#include "drivers/register_access.hh"
#include "drivers/stm32xx.h"

namespace mdrivlib
{

using RegisterDataT = uint32_t;

// Usage:
// SPI_<1>::CR1<SPI_CR1_SPE>.read()

template<unsigned PeriphNum>
struct SPI_ {
	static_assert(PeriphNum <= 5, "stm32f730 has only SPI1 - SPI5");

	static constexpr RegisterDataT BASE = PeriphNum == 1 ? SPI1_BASE
										: PeriphNum == 2 ? SPI2_BASE
										: PeriphNum == 3 ? SPI3_BASE
										: PeriphNum == 4 ? SPI4_BASE
										: PeriphNum == 5 ? SPI5_BASE
														 : SPI1_BASE;
	// clang-format off
	template <RegisterDataT Mask = 0xFFFFFFFF> using CR1 = RegisterBits<ReadWrite, BASE + offsetof(SPI_TypeDef, CR1), Mask>;
	template <RegisterDataT Mask = 0xFFFFFFFF> using CR2 = RegisterBits<ReadWrite, BASE + offsetof(SPI_TypeDef, CR2), Mask>;
    template <RegisterDataT Mask = 0xFFFFFFFF> using SR = RegisterBits<ReadWrite, BASE + offsetof(SPI_TypeDef, SR), Mask>;
    using DR = RegisterBits<ReadWrite, BASE + offsetof(SPI_TypeDef, DR), 0xFFFF>;
	template <RegisterDataT Mask = 0xFFFFFFFF> using CRCPOLY = RegisterBits<ReadWrite, BASE + offsetof(SPI_TypeDef, CRCPR), Mask>;
    template <RegisterDataT Mask = 0xFFFFFFFF> using RXCRC = RegisterBits<ReadWrite, BASE + offsetof(SPI_TypeDef, RXCRCR), Mask>;
    template <RegisterDataT Mask = 0xFFFFFFFF> using TXCRC = RegisterBits<ReadWrite, BASE + offsetof(SPI_TypeDef, TXCRCR), Mask>;
    template <RegisterDataT Mask = 0xFFFFFFFF> using I2SCFGR = RegisterBits<ReadWrite, BASE + offsetof(SPI_TypeDef, I2SCFGR), Mask>;
    template<RegisterDataT Mask = 0xFFFFFFFF> using I2SPR = RegisterBits<ReadWrite, BASE + offsetof(SPI_TypeDef, I2SPR), Mask>;
	// clang-format on
};

} // namespace mdrivlib
