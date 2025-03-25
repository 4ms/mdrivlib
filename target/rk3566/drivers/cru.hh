#pragma once
#include "drivers/periph_base_addr.hh"
#include "drivers/register_access.hh"
#include <cstdint>

namespace mdrivlib::RockchipPeriph
{

namespace Cru
{

struct CLKSEL {
	// CON00 is at the base address (offset 0x0100 in the TRM)
	// FIXME: there might be gaps, check!
	uint32_t con[85];

	enum Registers {
		//...
		I2S1_tx = 15,
		I2S1_tx_frac = 16,
		I2S1_rx = 17,
		I2S1_rx_frac = 18,
		//
	};

	constexpr static uint32_t reg(Registers regnum) {
		return CRU_CLKSEL_BASE + (regnum * 4);
	}
};

// Note: these names match what's in the TRM so you can search the PDF
// The enum names are the same, but with 'i2sN' => 'i2s' and removing _tx and _rx

// MCLKOUT
enum class i2s_mclkout_sel {
	mclk_i2s_8ch_tx = 0b0,
	xin_osc0_half = 0b1,
};
using i2s1_mclkout_tx_sel = RegisterMaskedChoice<CLKSEL::reg(CLKSEL::I2S1_tx), 1 << 15, i2s_mclkout_sel>;
using i2s1_mclkout_rx_sel = RegisterMaskedChoice<CLKSEL::reg(CLKSEL::I2S1_rx), 1 << 15, i2s_mclkout_sel>;

// MCLK Clock Mux
enum class mclk_i2s_8ch_sel {
	clk_i2s_8ch_src = 0b00,	 // e.g. clk_i2s1_8ch_tx_src or clk_i2s1_8ch_rx_src in TRM
	clk_i2s_8ch_frac = 0b01, // e.g. clk_i2s1_8ch_tx_frac or clk_i2s1_8ch_rx_frac in TRM
	i2s_mclkin = 0b10,		 // e.g. i2s1_mclkin in TRM
	xin_osc0_half = 0b11,
};
using mclk_i2s1_8ch_tx_sel = RegisterMaskedChoice<CLKSEL::reg(CLKSEL::I2S1_tx), (0b11 << 10), mclk_i2s_8ch_sel>;
using mclk_i2s1_8ch_rx_sel = RegisterMaskedChoice<CLKSEL::reg(CLKSEL::I2S1_rx), (0b11 << 10), mclk_i2s_8ch_sel>;

// Clk i2s Clock Mux
enum class clk_i2s_8ch_src_sel {
	clk_gpll_mux = 0b00,
	clk_cpll_mux = 0b01,
	clk_npll_mux = 0b10,
};
using i2s1_8ch_tx_src_sel = RegisterMaskedChoice<CLKSEL::reg(CLKSEL::I2S1_tx), (0b11 << 8), clk_i2s_8ch_src_sel>;
using i2s1_8ch_rx_src_sel = RegisterMaskedChoice<CLKSEL::reg(CLKSEL::I2S1_rx), (0b11 << 8), clk_i2s_8ch_src_sel>;

// I2s Clk divider -- "Divide clk_i2s1_8ch_tx_src by (div_con + 1)"
using i2s1_8ch_tx_src_div = RegisterMasked16<CLKSEL::reg(CLKSEL::I2S1_tx), (0b111'1111 << 0)>;
using i2s1_8ch_rx_src_div = RegisterMasked16<CLKSEL::reg(CLKSEL::I2S1_rx), (0b111'1111 << 0)>;

// i2s fractional clock divider
struct i2s1_8ch_tx_frac_div : RegisterBits<ReadWrite, CLKSEL::reg(CLKSEL::I2S1_tx_frac), 0xFFFFFFFF> {
	static void write(uint16_t numerator, uint16_t denominator) {
		RegisterBits::write((uint32_t)numerator << 16 | (uint32_t)denominator);
	}
};

struct i2s1_8ch_rx_frac_div : RegisterBits<ReadWrite, CLKSEL::reg(CLKSEL::I2S1_rx_frac), 0xFFFFFFFF> {
	static void write(uint16_t numerator, uint16_t denominator) {
		RegisterBits::write((uint32_t)numerator << 16 | (uint32_t)denominator);
	}
};

} // namespace Cru

} // namespace mdrivlib::RockchipPeriph
