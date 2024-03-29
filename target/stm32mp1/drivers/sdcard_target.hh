#pragma once
#include "drivers/rcc.hh"
#include "drivers/stm32xx.h"

namespace mdrivlib
{

template<typename ConfT>
struct SDMMCTarget {
	static void setup_hal_handle(SD_HandleTypeDef &hsd) {
		RCC->SDMMC12CKSELR = RCC_SDMMC12CLKSOURCE_HSI; //3 = HSI = 64MHz

		// From RefManual: SDMMC_CK frequency = sdmmc_ker_ck / [2 * CLKDIV]
		// And CLKDIV = 0 means no dividing
		uint32_t source_clock = 64'000'000;
		uint32_t clock_div = (ConfT::speed_hz == source_clock) ? 0 : source_clock / (2 * ConfT::speed_hz);
		if (clock_div > 0x3FF)
			clock_div = 0x3FF;

		hsd.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
		hsd.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
		hsd.Init.BusWide = ConfT::width ? SDMMC_BUS_WIDE_4B : SDMMC_BUS_WIDE_1B;
		hsd.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_ENABLE;
		hsd.Init.ClockDiv = clock_div;
		//MP1: source clk = 64M, base_clock = 32M, clock_div=2 => 16MHz, clock_div=1 => 32MHz
	}

	static bool read(SD_HandleTypeDef *hsd, uint8_t *data, uint32_t block_num, uint32_t numblocks) {
		if (HAL_SD_ReadBlocks(hsd, data, block_num, numblocks, 1000) != HAL_OK)
			return false;
		return true;
	}

	static bool write(SD_HandleTypeDef *hsd, uint8_t *data, uint32_t block_num, uint32_t numblocks) {
		if (HAL_SD_WriteBlocks(hsd, data, block_num, numblocks, 1000) != HAL_OK)
			return false;
		return true;
	}

	static void config_widebus(SD_HandleTypeDef *) {
		//Do nothing, HAL_SD_Init() calls HAL_SD_ConfigWideBusOperation
	}
};

} // namespace mdrivlib
