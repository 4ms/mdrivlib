#pragma once
#include "drivers/rcc.hh"
#include "stm32xx.h"
#include <optional>

namespace mdrivlib
{

template<typename ConfT>
struct SDMMCTarget {

	static void setup_hal_handle(SD_HandleTypeDef &hsd) {
		auto has_clock_div = SDMMCTarget<ConfT>::calc_clock_div();

		hsd.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
		hsd.Init.ClockBypass = has_clock_div.has_value() ? SDMMC_CLOCK_BYPASS_DISABLE : SDMMC_CLOCK_BYPASS_ENABLE;
		hsd.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
		hsd.Init.BusWide = ConfT::width ? SDMMC_BUS_WIDE_4B : SDMMC_BUS_WIDE_1B;
		hsd.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_ENABLE;
		hsd.Init.ClockDiv = has_clock_div.value_or(0);
		//F7: source_clk = 48M, clock_div=0 => 48/(2+0) = 24MHz, clock_div=1 => 48/(2+1) = 16MHz
	}

private:
	static std::optional<uint32_t> calc_clock_div() {
		uint32_t source_clock;
		//Clock sel bit selects PLL48 clock or system clock
		constexpr uint32_t cksel_bit = (ConfT::SDPeriphNum == 1) ? RCC_DCKCFGR2_SDMMC1SEL : RCC_DCKCFGR2_SDMMC2SEL;
		if ((RCC->DCKCFGR2 & cksel_bit) == 0)
			source_clock = 48'000'000;
		else
			source_clock = HAL_RCC_GetHCLKFreq(); //or HAL_RCC_GetSysClockFreq()

		if (ConfT::speed_hz == source_clock)
			return std::nullopt; //No clock divider

		// From RefManual: SDMMC_CK frequency = SDMMCCLK / [CLKDIV + 2]
		uint32_t clock_div_raw = source_clock / ConfT::speed_hz;
		if (clock_div_raw < 2)
			return 0;
		if (clock_div_raw > 257)
			return 255;
		return clock_div_raw - 2;
	}
};

} // namespace mdrivlib
