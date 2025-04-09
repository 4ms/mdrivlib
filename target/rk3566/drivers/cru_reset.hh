#pragma once
#include "drivers/periph_base_addr.hh"
#include "drivers/register_access.hh"
#include <cstdint>

namespace mdrivlib::RockchipPeriph
{

namespace Cru
{

struct SOFTRST {
	// CRU_SOFTRST_CON0 to CRU_SOFTRST_CON29:
	// offset 0x0400 to 0x0474
	constexpr static uint32_t reg(uint32_t regnum) {
		return CRU_SOFTRST_BASE + (regnum * 4);
	}
};

using mresetn_i2s3_2ch_rx = RegisterMasked16BitNum<SOFTRST::reg(5), 6>;
using mresetn_i2s3_2ch_tx = RegisterMasked16BitNum<SOFTRST::reg(5), 5>;
using mresetn_i2s2_2ch = RegisterMasked16BitNum<SOFTRST::reg(5), 4>;
using mresetn_i2s1_8ch_rx = RegisterMasked16BitNum<SOFTRST::reg(5), 3>;
using mresetn_i2s1_8ch_tx = RegisterMasked16BitNum<SOFTRST::reg(5), 2>;
using mresetn_i2s0_8ch_rx = RegisterMasked16BitNum<SOFTRST::reg(5), 1>;
using mresetn_i2s0_8ch_tx = RegisterMasked16BitNum<SOFTRST::reg(5), 0>;

using resetn_i2c5 = RegisterMasked16BitNum<SOFTRST::reg(22), 11>;
using presetn_i2c5 = RegisterMasked16BitNum<SOFTRST::reg(22), 10>;
using resetn_i2c4 = RegisterMasked16BitNum<SOFTRST::reg(22), 9>;
using presetn_i2c4 = RegisterMasked16BitNum<SOFTRST::reg(22), 8>;
using resetn_i2c3 = RegisterMasked16BitNum<SOFTRST::reg(22), 7>;
using presetn_i2c3 = RegisterMasked16BitNum<SOFTRST::reg(22), 6>;
using resetn_i2c2 = RegisterMasked16BitNum<SOFTRST::reg(22), 5>;
using presetn_i2c2 = RegisterMasked16BitNum<SOFTRST::reg(22), 4>;
using resetn_i2c1 = RegisterMasked16BitNum<SOFTRST::reg(22), 3>;
using presetn_i2c1 = RegisterMasked16BitNum<SOFTRST::reg(22), 2>;

using areset_mcu = RegisterMasked16BitNum<SOFTRST::reg(26), 10>;
} // namespace Cru

} // namespace mdrivlib::RockchipPeriph
