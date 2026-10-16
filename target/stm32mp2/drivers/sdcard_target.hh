#pragma once
#include "drivers/rcc.hh"
#include "drivers/rcc_xbar.hh"
#include "drivers/stm32xx.h"

// SDMMC target for STM32MP2. Same as MP1: blocking HAL reads/writes, with the CPU
// emptying the FIFO (no IDMA, so no RIF/RISAF setup is needed for DDR buffers).

namespace mdrivlib
{

template<typename ConfT>
struct SDMMCTarget {
	static_assert(ConfT::SDPeriphNum == 1, "Only SDMMC1 is supported (SDCard is hardcoded to SDMMC1)");

	static constexpr unsigned SDMMC1_FlexgenChan = 51;
	static constexpr uint32_t KernelClockHz = 64'000'000; // HSI

	static void setup_hal_handle(SD_HandleTypeDef &hsd) {
		// Kernel clock = HSI / 1. HAL_SD_Init() reads this back from RCC to set the 400kHz init clock.
		FlexbarConf{.PLL = FlexbarConf::PLLx::HSI, .findiv = 0, .prediv = 0}.init(SDMMC1_FlexgenChan);

		// SDMMC_CK = sdmmc_ker_ck / (2 * CLKDIV), and CLKDIV = 0 means no dividing
		uint32_t clock_div = (ConfT::speed_hz >= KernelClockHz) ? 0 : KernelClockHz / (2 * ConfT::speed_hz);
		if (clock_div > 0x3FF)
			clock_div = 0x3FF;

		hsd.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
		hsd.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
		hsd.Init.BusWide = ConfT::width ? SDMMC_BUS_WIDE_4B : SDMMC_BUS_WIDE_1B;
		hsd.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_ENABLE;
		hsd.Init.ClockDiv = clock_div;
	}

	static bool read(SD_HandleTypeDef *hsd, uint8_t *data, uint32_t block_num, uint32_t numblocks) {
		return HAL_SD_ReadBlocks(hsd, data, block_num, numblocks, 1000) == HAL_OK;
	}

	static bool write(SD_HandleTypeDef *hsd, uint8_t *data, uint32_t block_num, uint32_t numblocks) {
		return HAL_SD_WriteBlocks(hsd, data, block_num, numblocks, 1000) == HAL_OK;
	}

	static void config_widebus(SD_HandleTypeDef *) {
		//Do nothing, HAL_SD_Init() calls HAL_SD_ConfigWideBusOperation
	}
};

} // namespace mdrivlib
