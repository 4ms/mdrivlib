#pragma once
#include "drivers/dsi_panel.hh"

// =============================================================================
//  ER-TFT028-2 (BuyDisplay): 2.8" IPS, 480(H)x480(V), Sitronix ST7701S
//  controller, 2-lane MIPI-DSI (+ clock lane), RGB888, video mode.
// =============================================================================
// Verified on hardware (stm32mp2-baremetal ltdc-dsi-028/, ~59 Hz).
// The ST7701S supports 1 or 2 data lanes, max 550 Mbps/lane on 2 lanes.
//
// There is no vendor DSI timing table, but the vendor init sequence programs the
// panel's own timing generator (BK0):
//   LNESET  C0 = 3B 00 -> (0x3B+1)*8 = 480 lines
//   PORCTRL C1 = 10 0C -> VBP = 16, VFP = 12
//   INVSET  C2 = 07 0A -> RTNI = 0x0A -> minimum 512 + 10*16 = 672 pclk per line
// so the mode uses those vertical porches verbatim and an HTotal (680) just above
// the 672-clock minimum. If you change HFP/HBP or the vertical porches, change the
// BK0 C1/C2 values in the init sequence to match.
//
// Clocking chain (HSE = 40 MHz):
//   HSE 40 MHz --/2--> ck_ker_dsiphy 20 MHz --[D-PHY PLL]--> 400 Mbps/lane
//   lane byte clock = 400/8 = 50 MHz
//   DSI pixel throughput = 400*2/24 = 33.3 MHz (max)
//   LTDC pixel clock = PLL4/N <= 21 MHz: PLL4 = 1200 MHz -> /58 = 20.7 MHz
//     (680*512*60 Hz = 20.9 MHz). ~21 < 33 MHz is the headroom burst needs.
//
// D-PHY PLL: HAL_DSI_DT_400 row = {hs_freq 0x05, odf /2, vco 0x0F, prop 0x0B}:
//   f_vco = 20 * 80 / 4 = 400 MHz; data_rate = 2*20*80 / (4*2) = 400 Mbps/lane
// PHY timers: hstt_phy_141_table row HSTT(400, clk_lp2hs=59, clk_hs2lp=37,
// data_lp2hs=44, data_hs2lp=21).
//
// Verified (2026-09-10): burst = good picture; non-burst sync-events and
// sync-pulses = bad picture. LP in the horizontal porches MUST stay on: with it
// off the host's DPI pixel FIFO overflows every line (ISR1 LPWRE recurring) and
// the panel shows noise. The DSI stream must be started with the LTDC gated off
// and the host stopped before the video config is written (gated_video_start).
//
// Init sequence: converted from the vendor's "ER-TFT028-2_Initialization
// Program.txt" (41 DCS writes, one 120 ms delay after sleep-out). The
// {0xFF,0x77,0x01,0x00,0x00,bk} writes are the ST7701S "Command2 BKx" bank
// selects (0x10 BK0 display timing, 0x11 BK1 power/gamma, 0x13 BK3, 0x00 = back
// to Command1). The tail is sleep-out (0x11) + 120 ms, COLMOD 0x3A=0x77 (RGB888),
// MADCTL 0x36=0x00, TE on (0x35), display-on (0x29).
//
// Board note: the ER-TFT028-2 backlight is a bare 4-chip LED string (LEDA/LEDK,
// 60 mA typ), not an ~18 V string; check the boost driver suits it.

namespace mdrivlib::Panels::ST7701S
{
constexpr uint8_t DELAY = DsiPanel::InitSeqDelay;

// clang-format off
inline constexpr uint8_t init_seq[] = {
	6, 0xFF, 0x77, 0x01, 0x00, 0x00, 0x13,
	2, 0xEF, 0x08,
	6, 0xFF, 0x77, 0x01, 0x00, 0x00, 0x10,
	3, 0xC0, 0x3B, 0x00,
	3, 0xC1, 0x10, 0x0C,
	3, 0xC2, 0x07, 0x0A,
	2, 0xC7, 0x04,
	2, 0xCC, 0x10,
	2, 0xCD, 0x08,
	17, 0xB0, 0x05, 0x12, 0x98, 0x0E, 0x0F, 0x07, 0x07, 0x09, 0x09, 0x23, 0x05, 0x52, 0x0F, 0x67, 0x2C, 0x11,
	17, 0xB1, 0x0B, 0x11, 0x97, 0x0C, 0x12, 0x06, 0x06, 0x08, 0x08, 0x22, 0x03, 0x51, 0x11, 0x66, 0x2B, 0x0F,
	6, 0xFF, 0x77, 0x01, 0x00, 0x00, 0x11,
	2, 0xB0, 0x5D,
	2, 0xB1, 0x3E,
	2, 0xB2, 0x81,
	2, 0xB3, 0x80,
	2, 0xB5, 0x4E,
	2, 0xB7, 0x85,
	2, 0xB8, 0x20,
	2, 0xC1, 0x78,
	2, 0xC2, 0x78,
	2, 0xD0, 0x88,
	4, 0xE0, 0x00, 0x00, 0x02,
	12, 0xE1, 0x06, 0x30, 0x08, 0x30, 0x05, 0x30, 0x07, 0x30, 0x00, 0x33, 0x33,
	13, 0xE2, 0x11, 0x11, 0x33, 0x33, 0xF4, 0x00, 0x00, 0x00, 0xF4, 0x00, 0x00, 0x00,
	5, 0xE3, 0x00, 0x00, 0x11, 0x11,
	3, 0xE4, 0x44, 0x44,
	17, 0xE5, 0x0D, 0xF5, 0x30, 0xF0, 0x0F, 0xF7, 0x30, 0xF0, 0x09, 0xF1, 0x30, 0xF0, 0x0B, 0xF3, 0x30, 0xF0,
	5, 0xE6, 0x00, 0x00, 0x11, 0x11,
	3, 0xE7, 0x44, 0x44,
	17, 0xE8, 0x0C, 0xF4, 0x30, 0xF0, 0x0E, 0xF6, 0x30, 0xF0, 0x08, 0xF0, 0x30, 0xF0, 0x0A, 0xF2, 0x30, 0xF0,
	3, 0xE9, 0x36, 0x01,
	8, 0xEB, 0x00, 0x01, 0xE4, 0xE4, 0x44, 0x88, 0x40,
	17, 0xED, 0xFF, 0x10, 0xAF, 0x76, 0x54, 0x2B, 0xCF, 0xFF, 0xFF, 0xFC, 0xB2, 0x45, 0x67, 0xFA, 0x01, 0xFF,
	7, 0xEF, 0x08, 0x08, 0x08, 0x45, 0x3F, 0x54,
	6, 0xFF, 0x77, 0x01, 0x00, 0x00, 0x00,
	1, 0x11,
	DELAY, 120,
	2, 0x3A, 0x77,
	2, 0x36, 0x00,
	2, 0x35, 0x00,
	1, 0x29,
};
// clang-format on

} // namespace mdrivlib::Panels::ST7701S

namespace mdrivlib::Panels
{

inline constexpr DsiPanel ER_TFT028_2{
	.name = "ER-TFT028-2 (ST7701S)",

	.h_active = 480,
	.h_front = 140,
	.h_sync = 10,
	.h_back = 50, // HTotal 680 >= 672 (INVSET RTNI minimum)
	.v_active = 480,
	.v_front = 12, // = PORCTRL VFP
	.v_sync = 4,
	.v_back = 16, // = PORCTRL VBP
	.dsi_vbp_offset = 0,

	.pixel_clk_max_hz = 21'000'000,

	.num_lanes = 2,
	.lane_rate_mbps = 400,
	.pll_ref_khz = 20'000,
	.pll_ndiv = 80,
	.pll_idf = 4,
	.pll_odf = 1, // /2
	.phy_data_rate = HAL_DSI_DT_400,
	.tx_escape_clk_div = 4, // 50 / 4 = 12.5 MHz

	.phy_timers =
		{
			.clk_lp2hs = 59,
			.clk_hs2lp = 37,
			.data_lp2hs = 44,
			.data_hs2lp = 21,
			.stop_wait = 10,
		},

	.video_mode = DSI_VID_MODE_BURST,
	.lp_during_active_hblank = true,
	.lp_largest_packet = 4,
	.lp_vact_largest_packet = 0,

	.gated_video_start = true,
	.enable_bta = true, // the PoC enabled BTA + ECC/CRC RX for panel read-back

	// RESX is active-low: pulse >= 10 us (TRW), then wait for the reset-cancel/NVM
	// load (TRT, 5 ms) before the first command. 120 ms covers the worst case
	// (reset while in sleep-out blanks for up to 120 ms).
	.reset_pre_deassert_us = 1'000,
	.reset_assert_us = 10'000,
	.reset_settle_us = 120'000,

	.init_seq = ST7701S::init_seq,
};

static_assert(ER_TFT028_2.is_valid());
static_assert(ER_TFT028_2.num_init_writes() == 41);
static_assert(ER_TFT028_2.h_total() == 680 && ER_TFT028_2.v_total() == 512);

} // namespace mdrivlib::Panels
