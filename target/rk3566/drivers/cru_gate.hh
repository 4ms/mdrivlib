#pragma once
#include "drivers/periph_base_addr.hh"
#include "drivers/register_access.hh"
#include <cstdint>

namespace mdrivlib::RockchipPeriph
{

namespace CruGate
{

// CRU_GATE_CON0 to CRU_GATE_CON35:
// offset 0x0300 to 0x038C

enum Registers {
	//...
	CON05 = 5,
	CON06 = 6,
	//...
};

constexpr static uint32_t CON(Registers regnum) {
	return CRU_GATE_BASE + (regnum * 4);
}

constexpr static uint32_t CON(uint32_t regnum) {
	return CRU_GATE_BASE + (regnum * 4);
}

enum cru_gate {
	clock_enable = 0,
	clock_disable = 1,
};

using hclk_i2s3_2ch_en = RegisterMaskedChoice<CON(5), 1, 13, cru_gate>;
using hclk_i2s2_2ch_en = RegisterMaskedChoice<CON(5), 1, 12, cru_gate>;
using hclk_i2s1_8ch_en = RegisterMaskedChoice<CON(5), 1, 11, cru_gate>;
using hclk_i2s0_8ch_en = RegisterMaskedChoice<CON(5), 1, 10, cru_gate>;
using hclk_gic_audio_en = RegisterMaskedChoice<CON(5), 1, 1, cru_gate>;

using mclk_i2s1_8ch_tx_en = RegisterMaskedChoice<CON(6), 1, 10, cru_gate>;
using i2s1_mclkout_tx_en = RegisterMaskedChoice<CON(6), 1, 11, cru_gate>;

using clk_i2c5_en = RegisterMaskedChoice<CON(30), 1, 9, cru_gate>;
using pclk_i2c5_en = RegisterMaskedChoice<CON(30), 1, 8, cru_gate>;
using clk_i2c4_en = RegisterMaskedChoice<CON(30), 1, 7, cru_gate>;
using pclk_i2c4_en = RegisterMaskedChoice<CON(30), 1, 6, cru_gate>;
using clk_i2c3_en = RegisterMaskedChoice<CON(30), 1, 5, cru_gate>;
using pclk_i2c3_en = RegisterMaskedChoice<CON(30), 1, 4, cru_gate>;
using clk_i2c2_en = RegisterMaskedChoice<CON(30), 1, 3, cru_gate>;
using pclk_i2c2_en = RegisterMaskedChoice<CON(30), 1, 2, cru_gate>;
using clk_i2c1_en = RegisterMaskedChoice<CON(30), 1, 1, cru_gate>;
using pclk_i2c1_en = RegisterMaskedChoice<CON(30), 1, 0, cru_gate>;

using clk_i2c_en = RegisterMaskedChoice<CON(32), 1, 10, cru_gate>;

} // namespace CruGate

} // namespace mdrivlib::RockchipPeriph
