#pragma once
#include "drivers/periph_base_addr.hh"
#include "drivers/register_access.hh"
#include <cstdint>

namespace mdrivlib::RockchipPeriph
{

namespace CruClksel
{

// CRU_CLKSEL_CON0 to CRU_CLKSEL_CON84:
// offset 0x0100 to 0x0250
enum Registers {
	CON10 = 10,

	//...
	I2S1_tx = 15,
	I2S1_tx_frac = 16,
	I2S1_rx = 17,
	I2S1_rx_frac = 18,
	//

};

constexpr static uint32_t CON(Registers regnum) {
	return CRU_CLKSEL_BASE + (regnum * 4);
}

constexpr static uint32_t CON(uint32_t regnum) {
	return CRU_CLKSEL_BASE + (regnum * 4);
}

// Note: the alias names (such as `i2s1_mclkout_tx_sel`) match what's in the TRM so you can search the PDF.
//
// The enum names are modified so they can be re-used for multiple instances of the peripheral:
//    e.g. 'i2s1' => 'i2s', and remove `_tx` and `_rx`

// MCLKOUT
enum class i2s_mclkout_sel {
	mclk_i2s_8ch = 0b0,
	xin_osc0_half = 0b1,
};
using i2s1_mclkout_tx_sel = RegisterMaskedChoice<CON(I2S1_tx), 1, 15, i2s_mclkout_sel>;
using i2s1_mclkout_rx_sel = RegisterMaskedChoice<CON(I2S1_rx), 1, 15, i2s_mclkout_sel>;

// MCLK Clock Mux
enum class mclk_i2s_8ch_sel {
	clk_i2s_8ch_src = 0b00,	 // e.g. clk_i2s1_8ch_tx_src or clk_i2s1_8ch_rx_src in TRM
	clk_i2s_8ch_frac = 0b01, // e.g. clk_i2s1_8ch_tx_frac or clk_i2s1_8ch_rx_frac in TRM
	i2s_mclkin = 0b10,		 // e.g. i2s1_mclkin in TRM
	xin_osc0_half = 0b11,
};
using mclk_i2s1_8ch_tx_sel = RegisterMaskedChoice<CON(I2S1_tx), 0b11, 10, mclk_i2s_8ch_sel>;
using mclk_i2s1_8ch_rx_sel = RegisterMaskedChoice<CON(I2S1_rx), 0b11, 10, mclk_i2s_8ch_sel>;

// Clk i2s Clock Mux
enum class clk_i2s_8ch_src_sel {
	clk_gpll_mux = 0b00,
	clk_cpll_mux = 0b01,
	clk_npll_mux = 0b10,
};
using i2s1_8ch_tx_src_sel = RegisterMaskedChoice<CON(I2S1_tx), 0b11, 8, clk_i2s_8ch_src_sel>;
using i2s1_8ch_rx_src_sel = RegisterMaskedChoice<CON(I2S1_rx), 0b11, 8, clk_i2s_8ch_src_sel>;

// I2s Clk divider -- "Divide clk_i2s1_8ch_tx_src by (div_con + 1)"
using i2s1_8ch_tx_src_div = RegisterMasked16<CON(I2S1_tx), 0b111'1111, 0>;
using i2s1_8ch_rx_src_div = RegisterMasked16<CON(I2S1_rx), 0b111'1111, 0>;

// i2s fractional clock divider
struct i2s1_8ch_tx_frac_div : RegisterBits<ReadWrite, CON(I2S1_tx_frac), 0xFFFFFFFF> {
	static void write(uint16_t numerator, uint16_t denominator) {
		RegisterBits::write((uint32_t)numerator << 16 | (uint32_t)denominator);
	}
};

struct i2s1_8ch_rx_frac_div : RegisterBits<ReadWrite, CON(I2S1_rx_frac), 0xFFFFFFFF> {
	static void write(uint16_t numerator, uint16_t denominator) {
		RegisterBits::write((uint32_t)numerator << 16 | (uint32_t)denominator);
	}
};

// gic_audio low-level clocks:
enum class hclk_gic_audio_clock_mux {
	clk_gpll_div_150m = 0b00,
	clk_gpll_div_100m = 0b01,
	clk_gpll_div_75m = 0b10,
	xin_osc0_func_mux = 0b11,
};
using hclk_gic_audio_sel = RegisterMaskedChoice<CON(10), 0b11, 10, hclk_gic_audio_clock_mux>;

enum class clk_i2c_clock_mux {
	clk_gpll_div_200m = 0b00,
	clk_gpll_div_100m = 0b01,
	xin_osc0_func_mux = 0b10,
	clk_cpll_div_100m = 0b11,
};
using clk_i2c_sel = RegisterMaskedChoice<CON(71), 0b11, 8, clk_i2c_clock_mux>;

} // namespace CruClksel

} // namespace mdrivlib::RockchipPeriph
