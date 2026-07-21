#pragma once
#include "drivers/rcc_common.hh"
#include <cstdint>

namespace mdrivlib
{

namespace RCC_Clocks
{

// clang-format off
template<typename PLLT> struct MuxSel {};
template<> struct MuxSel<PLL1> { using Mux = MuxSelPLL1; };
template<> struct MuxSel<PLL2> { using Mux = MuxSelPLL2; };
template<> struct MuxSel<PLL3> { using Mux = MuxSelPLL3; };
template<> struct MuxSel<PLL4> { using Mux = MuxSelPLL4; };
template<> struct MuxSel<PLL5> { using Mux = MuxSelPLL5; };
template<> struct MuxSel<PLL6> { using Mux = MuxSelPLL6; };
template<> struct MuxSel<PLL7> { using Mux = MuxSelPLL7; };
template<> struct MuxSel<PLL8> { using Mux = MuxSelPLL8; };
// clang-format on

struct PLLSettings {
	MuxSelSource src = MuxSelSource::hse;
	uint16_t mult = 1;	  // 12 valid bits
	uint8_t refdiv = 1;	  // 7 valid bits
	uint8_t postdiv1 = 1; // 3 valid bits
	uint8_t postdiv2 = 1; // 3 valid bits
	uint32_t frac = 0;	  // 24 valid bits

	constexpr uint32_t calc_freq(uint32_t ref_clk) const {
		return (float(ref_clk) / refdiv) * (float(mult) + frac / float(0x1000000)) / (postdiv1 * postdiv2);
	}
};

template<typename PLLT>
inline void set_pll(PLLSettings v) {
	PLLT::Enable::clear();
	while (PLLT::Enable::read())
		;

	MuxSel<PLLT>::Mux::write(v.src);
	PLLT::FBDIVMult::write(v.mult);
	PLLT::FREFDiv::write(v.refdiv);
	PLLT::PostDiv1::write(v.postdiv1);
	PLLT::PostDiv2::write(v.postdiv2);
	PLLT::PostDivEnable::set();
	PLLT::FracMult::write(v.frac);

	PLLT::Enable::set();
	while (!PLLT::Enable::read())
		;
}

template<typename PLLT>
inline PLLSettings get_pll_settings() {
	PLLSettings v;
	auto mux = MuxSel<PLLT>::Mux::read();
	v.src = mux == 0 ? MuxSelSource::hsi
		  : mux == 1 ? MuxSelSource::hse
		  : mux == 2 ? MuxSelSource::msi
					 : MuxSelSource::hsi; // default to hsi on invalid value
	v.mult = PLLT::FBDIVMult::read();
	v.refdiv = PLLT::FREFDiv::read();
	v.postdiv1 = PLLT::PostDiv1::read();
	v.postdiv2 = PLLT::PostDiv2::read();
	v.frac = PLLT::FracMult::read();
	return v;
}

template<typename PLLT>
inline void print_pll_settings(auto printf_) {
	PLLSettings pll = get_pll_settings<PLLT>();
	printf_("PLL: %s /%d x%d /%d /%d frac:%u\n",
			pll.src == RCC_Clocks::MuxSelSource::hsi   ? "hsi"
			: pll.src == RCC_Clocks::MuxSelSource::hse ? "hse"
			: pll.src == RCC_Clocks::MuxSelSource::msi ? "msi"
													   : "???",
			pll.refdiv,
			pll.mult,
			pll.postdiv1,
			pll.postdiv2,
			pll.frac);
}

} // namespace RCC_Clocks

} // namespace mdrivlib
