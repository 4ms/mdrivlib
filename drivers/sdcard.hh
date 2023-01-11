#pragma once
#include "drivers/sdcard_conf.hh"
#include "stm32xx.h"

namespace mdrivlib
{

template<SDCardConfC ConfT>
struct SDCard {
	SD_HandleTypeDef hsd;

	SDCard() = default;

	void init() {
		RCC->SDMMC12CKSELR = 3; // HSI = 64MHz. Default value (just showing it here for educational purposes)

		HAL_SD_DeInit(&hsd);
		hsd.Instance = SDMMC1;
		hsd.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
		hsd.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
		hsd.Init.BusWide = SDMMC_BUS_WIDE_4B;
		hsd.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
		hsd.Init.ClockDiv = 2; // 64MHz/2 / 2 = 16MHz, seems to be the max OSD32-BRK can handle reliably

		Pin{ConfT::D0, PinMode::Alt};
		Pin{ConfT::D1, PinMode::Alt};
		Pin{ConfT::D2, PinMode::Alt};
		Pin{ConfT::D3, PinMode::Alt};
		Pin{ConfT::CMD, PinMode::Alt};
		Pin{ConfT::SCLK, PinMode::Alt};
	}
};

} // namespace mdrivlib
