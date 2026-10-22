#pragma once
#include "drivers/stm32xx.h"
#include <cstdint>

// RISAF4: the DDR firewall. TF-A configures it at boot; these helpers add regions at runtime
// (only possible from a secure context, and only if TF-A didn't set the global lock).
// Pattern verified in stm32mp2-baremetal usb-drd (dwc3_baremetal.cc), where added regions
// overlap TF-A's region 1.
//
// RIF compartment IDs: CID1 = Cortex-A35, CID2 = Cortex-M33, CID3 = Cortex-M0+.
// Avoid the region indices TF-A uses (its dts uses RISAF_REG_ID 1, 7, 8).

namespace mdrivlib::RISAF
{
constexpr uint32_t CID_A35 = 1u << 1;
constexpr uint32_t CID_M33 = 1u << 2;

constexpr uintptr_t DDRBase = 0x80000000;

// Add (or replace) a RISAF4 region for DDR [start, end_inclusive].
// Returns false if RISAF4 is globally locked.
inline bool add_ddr_region(
	unsigned reg_index, uintptr_t start, uintptr_t end_inclusive, uint32_t read_cids, uint32_t write_cids, bool secure) {
	if (RISAF4->CR & RISAF_CR_GLOCK)
		return false;

	auto &reg = RISAF4->REG[reg_index];
	reg.CFGR &= ~RISAF_REGCFGR_BREN;
	reg.STARTR = start - DDRBase;
	reg.ENDR = end_inclusive - DDRBase;
	reg.CIDCFGR = ((read_cids << RISAF_REGCIDCFGR_RDEN_Pos) & RISAF_REGCIDCFGR_RDEN_Msk) |
				  ((write_cids << RISAF_REGCIDCFGR_WREN_Pos) & RISAF_REGCIDCFGR_WREN_Msk);
	reg.CFGR = RISAF_REGCFGR_BREN | (secure ? RISAF_REGCFGR_SEC : 0);
	return true;
}

// Disable a region added with add_ddr_region().
// Returns false if RISAF4 is globally locked.
inline bool remove_ddr_region(unsigned reg_index) {
	if (RISAF4->CR & RISAF_CR_GLOCK)
		return false;

	RISAF4->REG[reg_index].CFGR &= ~RISAF_REGCFGR_BREN;
	return true;
}
} // namespace mdrivlib::RISAF
