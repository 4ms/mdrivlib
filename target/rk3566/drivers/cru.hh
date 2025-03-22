#include "masks.hh"
#include <stdint.h> // clangd complains about cstdint????

namespace RockchipPeriph::Cru
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

	void i2s1_mclkout_tx(bool enable) {
		con[I2S1_tx] = enable ? masked_set_bit(15) : masked_clr_bit(15);
	}
	void i2s1_mclkout_rx(bool enable) {
		con[I2S1_rx] = enable ? masked_set_bit(15) : masked_clr_bit(15);
	}

	enum mclk_i2s1_8ch_src {
		clk_i2s1_8ch = 0b00,
		clk_i2s1_8ch_frac = 0b01,
		i2s1_mclkin = 0b10,
		xin_osc0_half = 0b11,
	};
	void mclk_i2s1_8ch_tx_sel(enum mclk_i2s1_8ch_src source) {
		con[I2S1_tx] = masked(0b11 << 10, source << 10);
	}
	void mclk_i2s1_8ch_rx_sel(enum mclk_i2s1_8ch_src source) {
		con[I2S1_rx] = masked(0b11 << 10, source << 10);
	}

	enum clk_i2s1_8ch_src { gpll = 0b00, cpll = 0b00, npll = 0b00 };

	void i2s1_8ch_tx_src_sel(enum clk_i2s1_8ch_src source) {
		con[I2S1_tx] = masked(0b11 << 8, source << 8);
	}
	void i2s1_8ch_rx_src_sel(enum clk_i2s1_8ch_src source) {
		con[I2S1_rx] = masked(0b11 << 8, source << 8);
	}

	void i2s1_8ch_tx_src_div(uint8_t div_con) {
		// Divide clk_i2s1_8ch_tx_src by (div_con + 1)
		con[I2S1_tx] = masked(0b111111 << 0, div_con << 0);
	}
	void i2s1_8ch_rx_src_div(uint8_t div_con) {
		// Divide clk_i2s1_8ch_rx_src by (div_con + 1)
		con[I2S1_rx] = masked(0b111111 << 0, div_con << 0);
	}

	void i2s1_8ch_tx_frac_div(uint16_t num, uint16_t denom) {
		con[I2S1_tx_frac] = (uint32_t)num << 16 | (uint32_t)denom;
	}
	void i2s1_8ch_rx_frac_div(uint16_t num, uint16_t denom) {
		con[I2S1_rx_frac] = (uint32_t)num << 16 | (uint32_t)denom;
	}
};

} // namespace RockchipPeriph::Cru

namespace HW
{

static inline volatile RockchipPeriph::Cru::CLKSEL *ClkSel =
	reinterpret_cast<RockchipPeriph::Cru::CLKSEL *>(0xFDD20100);

}
