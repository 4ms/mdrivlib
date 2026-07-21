#pragma once
#include "stm32mp2xx_hal.h"
#include <utility>

namespace mdrivlib
{

struct FlexbarConf {
	enum class PLLx {
		_4 = RCC_XBAR_CLKSRC_PLL4,
		_5 = RCC_XBAR_CLKSRC_PLL5,
		_6 = RCC_XBAR_CLKSRC_PLL6,
		_7 = RCC_XBAR_CLKSRC_PLL7,
		_8 = RCC_XBAR_CLKSRC_PLL8,
	};
	PLLx PLL = PLLx::_4;
	uint8_t findiv = 0x3F;
	unsigned prediv = 0;

	enum FlexbarChan {
		SAI2_ = 24,
	};

	void init(unsigned chan_num) {
		if (chan_num >= 64)
			return;

		RCC->PREDIVxCFGR[chan_num] = (prediv & 0x3F);

		RCC->XBARxCFGR[chan_num] = RCC_XBARxCFGR_XBARxEN | std::to_underlying(PLL);
		while (RCC->XBARxCFGR[chan_num] & RCC_XBARxCFGR_XBARxSTS) // 1 = ongoing change
			;

		RCC->FINDIVxCFGR[chan_num] = RCC_FINDIVxCFGR_FINDIVxEN | (findiv & 0x3F);

		if (chan_num < 32) {
			while (RCC->FINDIVSR1 & (1 << chan_num))
				;
		} else if (chan_num < 64) {
			while (RCC->FINDIVSR2 & (1 << (chan_num - 32)))
				;
		}

		return;
	}
};

} // namespace mdrivlib
