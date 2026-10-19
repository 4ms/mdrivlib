#pragma once
#include "drivers/pin.hh"
#include "drivers/rcc_xbar.hh"
#include <cstdint>

namespace mdrivlib
{

// Serial NOR flash on an OCTOSPI, in indirect mode. See ospi_flash_driver.hh
struct OSPIFlashConfig {
	PinDef io0{};
	PinDef io1{};
	PinDef io2{};
	PinDef io3{};
	PinDef clk{};
	PinDef cs{};

	// OCTOSPI1 or OCTOSPI2. The IO manager is left in direct mode: OCTOSPIn uses IO port n
	unsigned ospi_num = 1;

	// Kernel clock (flexgen channel 48 for OCTOSPI1, 49 for OCTOSPI2). Default is HSI = 64MHz
	FlexbarConf kernel_clock{.PLL = FlexbarConf::PLLx::HSI, .findiv = 0, .prediv = 0};

	// Flash clock = kernel clock / clock_division (1 to 256)
	uint32_t clock_division = 2;

	uint32_t flash_size_bytes = 0x0100'0000;

	enum IOMode { SingleSPI, QuadSPI };
	IOMode io_mode = QuadSPI;
};

} // namespace mdrivlib
