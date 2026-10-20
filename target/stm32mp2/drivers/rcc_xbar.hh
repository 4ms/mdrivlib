#pragma once
#include "stm32mp2xx_hal.h"
#include <cstddef>
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
		HSI = RCC_XBAR_CLKSRC_HSI, // not a PLL, but a valid flexgen source
		HSE = RCC_XBAR_CLKSRC_HSE, // not a PLL, but a valid flexgen source
	};
	PLLx PLL = PLLx::_4;
	uint8_t findiv = 0x3F;
	unsigned prediv = 0;

	enum FlexbarChan {
		SAI2_ = 24,
		LTDC_ = 27,	  // ck_ker_ltdc: LTDC pixel clock (when SYSCFG DISPLAYCLKCR selects it)
		DSIPHY_ = 28, // ck_ker_dsiphy: DSI D-PHY PLL reference (17-27 MHz)
	};

	// Returns false if a status bit didn't clear (the channel may not be running at the new settings)
	bool init(unsigned chan_num) {
		if (chan_num >= 64)
			return false;

		// If the channel is using a PLL that's stopped, then we need to enable that PLL
		// before switching to a new one
		auto old_pll = pll_cfgr1(RCC->XBARxCFGR[chan_num] & RCC_XBARxCFGR_XBARxSEL_Msk);
		const bool started_old_pll = old_pll && !(*old_pll & RCC_PLL4CFGR1_PLLEN);
		if (started_old_pll) {
			*old_pll |= RCC_PLL4CFGR1_PLLEN;
			wait_while([=] { return !(*old_pll & RCC_PLL4CFGR1_PLLRDY); });
		}

		const uint32_t chan_bit = 1u << (chan_num % 32);
		auto &predivsr = chan_num < 32 ? RCC->PREDIVSR1 : RCC->PREDIVSR2;
		auto &findivsr = chan_num < 32 ? RCC->FINDIVSR1 : RCC->FINDIVSR2;

		RCC->PREDIVxCFGR[chan_num] = (prediv & 0x3F);
		bool ok = wait_while([&] { return predivsr & chan_bit; });

		RCC->FINDIVxCFGR[chan_num] = RCC_FINDIVxCFGR_FINDIVxEN | (findiv & 0x3F);
		ok &= wait_while([&] { return findivsr & chan_bit; });

		RCC->XBARxCFGR[chan_num] = RCC_XBARxCFGR_XBARxEN | std::to_underlying(PLL);
		ok &= wait_while([=] { return RCC->XBARxCFGR[chan_num] & RCC_XBARxCFGR_XBARxSTS; }); // 1 = ongoing change

		// If we started it, stop it again (unless it's the new source)
		if (started_old_pll && old_pll != pll_cfgr1(std::to_underlying(PLL)))
			*old_pll &= ~RCC_PLL4CFGR1_PLLEN;

		return ok;
	}

private:
	// PLLxCFGR1 of crossbar source src (PLL4 to PLL8), or nullptr if src isn't a PLL
	static volatile uint32_t *pll_cfgr1(uint32_t src) {
		switch (src) {
			case RCC_XBAR_CLKSRC_PLL4:
				return &RCC->PLL4CFGR1;
			case RCC_XBAR_CLKSRC_PLL5:
				return &RCC->PLL5CFGR1;
			case RCC_XBAR_CLKSRC_PLL6:
				return &RCC->PLL6CFGR1;
			case RCC_XBAR_CLKSRC_PLL7:
				return &RCC->PLL7CFGR1;
			case RCC_XBAR_CLKSRC_PLL8:
				return &RCC->PLL8CFGR1;
			default:
				return nullptr;
		}
	}

	static bool wait_while(auto busy) {
		int timeout = 1'000'000;
		while (busy()) {
			if (--timeout <= 0)
				return false;
		}
		return true;
	}
};

} // namespace mdrivlib
