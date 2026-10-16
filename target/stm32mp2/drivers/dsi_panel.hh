#pragma once
#include "stm32mp2xx_hal.h"
#include <cstddef>
#include <cstdint>
#include <span>

// Panel definition for the MIPI-DSI + LTDC display driver (drivers/dsi_screen.hh).
//
// Everything that belongs to the *panel* (resolution, porches, DSI lane count and
// rate, D-PHY PLL solution, PHY LP<->HS timers, video-mode and LP flags, pixel
// clock target, reset timing, controller init sequence) lives here, as constexpr
// data. Everything that belongs to the *board* (reset/backlight GPIOs, HSE
// frequency, pixel format, IRQ priority) lives in DsiScreenConf.
//
// See drivers/panels/ for verified panel definitions. To add a panel, copy one
// and change it as a set: lane_rate_mbps, pll_*, phy_data_rate and phy_timers all
// belong to the same row of ST's tables (see the comments in the panels/ files).
//
// Ported from the stm32mp2-baremetal ltdc-dsi/ (ILI9881C) and ltdc-dsi-028/
// (ST7701S) examples, both verified on hardware.

namespace mdrivlib
{

struct DsiPanel {
	const char *name = "";

	// ---- Display mode (pixels / lines), DRM-style ------------------------------
	uint32_t h_active = 0;
	uint32_t h_front = 0; // HFP
	uint32_t h_sync = 0;  // HSA
	uint32_t h_back = 0;  // HBP
	uint32_t v_active = 0;
	uint32_t v_front = 0; // VFP
	uint32_t v_sync = 0;  // VSA
	uint32_t v_back = 0;  // VBP

	// The DSI regenerates the vertical frame from its own VBP/VFP and can land one
	// line out of phase with the LTDC's DE (symptom: image shifted up by one line,
	// top row wraps to the bottom). This offset nudges ONLY the DSI's VBP/VFP (the
	// LTDC and the frame total are untouched, so refresh rate is unchanged): +1
	// shifts the image DOWN one line. 0 = matched to the LTDC.
	int32_t dsi_vbp_offset = 0;

	// ---- LTDC pixel clock ------------------------------------------------------
	// The driver picks the largest HSE-locked clock <= this: PLL4/N if PLL4 runs
	// from HSE, else HSE/N. It must be phase-locked to the DSI byte clock (which is
	// HSE-derived via ck_ker_dsiphy) or the video FIFO slips and the colours cycle,
	// and it must be BELOW the DSI pixel throughput (lane_rate * lanes / 24) so
	// burst mode has headroom. Never use the DSI-PLL loopback (DISPLAYCLKCR=0b00):
	// that locks pixel_clk to the lane rate with zero headroom, which forced LP on
	// every line and wrapped the image.
	uint32_t pixel_clk_max_hz = 0;

	// ---- DSI link --------------------------------------------------------------
	uint32_t num_lanes = 0;		 // 1..4 data lanes
	uint32_t lane_rate_mbps = 0; // per-lane HS data rate

	// D-PHY PLL. Reference = ck_ker_dsiphy, which the driver sets to HSE/2.
	//   f_vco     = f_ref * NDIV / IDF                (band 320..1250 MHz)
	//   data_rate = 2 * f_ref * NDIV / (IDF * ODF)    (x2 = DDR)
	// PllOdf must equal the odf column of the HAL_DSI_DT_xxx row selected by
	// phy_data_rate, so f_vco lands where that row's vco code is tuned. Keeping
	// f_vco/vco consistent is what makes the PLL lock.
	uint32_t pll_ref_khz = 20'000; // must be 17..27 MHz and equal the actual ck_ker_dsiphy
	uint32_t pll_ndiv = 0;		   // WRPCR0.NDIV (64..625)
	uint32_t pll_idf = 0;		   // WRPCR0.IDF  (1..16)
	uint32_t pll_odf = 0;		   // WRPCR1.ODF: 0/1/2/3 = /1,/2,/4,/8

	// Row of ST's D-PHY regulation table (hs_freq/vco/prop) used by HAL_DSI_Init.
	HAL_DSI_DataRate phy_data_rate = HAL_DSI_DT_80;

	// TX escape clock = lane_byte_clock / div, must be <= 20 MHz.
	uint32_t tx_escape_clk_div = 4;

	// PHY LP<->HS transition timers in lane-byte-clock cycles
	// (HAL_DSI_ConfigPhyTimer), from ST's hstt_phy_141_table (dw_mipi_dsi-stm.c,
	// HWVER_141 = MP25) row for lane_rate_mbps. Getting these right (esp.
	// data_lp2hs) is what makes the LP->HS transition into each active line land
	// cleanly instead of disturbing it.
	struct PhyTimers {
		uint32_t clk_lp2hs;
		uint32_t clk_hs2lp;
		uint32_t data_lp2hs;
		uint32_t data_hs2lp;
		uint32_t stop_wait;
	} phy_timers{};

	// ---- DSI video mode --------------------------------------------------------
	// DSI_VID_MODE_NB_PULSES / NB_EVENTS / BURST. Burst: each active line is
	// buffered and sent as one HS burst, which decouples the DSI byte clock from
	// the (independent) LTDC pixel clock -- required since they are not
	// frequency-locked. Non-burst drifts the pixels line-to-line.
	uint32_t video_mode = DSI_VID_MODE_BURST;

	// LP during vertical blanking (VSA/VBP/VFP) is always enabled by the driver:
	// the ILI9881C uses it as its frame delimiter (holding vblank in HS blanks the
	// panel) and it's the standard video-mode behaviour.
	//
	// LP during the horizontal porches of active lines (LPHFPE/LPHBPE/LPVAE):
	// true = drop to LP in HFP/HBP every line. Both verified panels need true.
	bool lp_during_active_hblank = true;

	// LP packet-size limits during blanking (HAL_DSI_ConfigVideoMode LPMCR).
	uint32_t lp_largest_packet = 4;
	uint32_t lp_vact_largest_packet = 0;

	// Start the DSI video stream only with the LTDC gated off, and fully stop the
	// host before writing its video config. Starting the host while the LTDC is
	// already scanning leaves a partial pixel in the host's DPI FIFO and the whole
	// stream comes out offset by 1-2 bytes (red shows as green, image wrapped),
	// differently on every restart; and writing the video config into a host
	// already enabled for LP commands left the stream blank until the host was
	// stop/started. Required by the ST7701S. The ILI9881C was verified with the
	// simpler sequence (false): video config written into the running host, LTDC
	// already scanning.
	bool gated_video_start = false;

	// Enable bus turnaround + ECC/CRC checking of returned packets
	// (HAL_DSI_ConfigFlowControl BTA|ECC_RX|CRC_RX). Needed for DsiScreen::dcs_read().
	bool enable_bta = false;

	// ---- Panel hardware reset ----------------------------------------------------
	uint32_t reset_pre_deassert_us = 0; // if non-zero: deassert reset and wait this long before the pulse
	uint32_t reset_assert_us = 10'000;
	uint32_t reset_settle_us = 120'000; // after releasing reset, before the first command

	// ---- Controller init sequence ----------------------------------------------
	// Flat stream of records:  [len][len bytes...]  sent as one DCS write in LP
	// (1 byte -> DCS short write no-param 0x05, 2 bytes -> short write 1-param
	// 0x15, >2 bytes -> DCS long write 0x39). A record whose length byte is
	// InitSeqDelay (0xFF) is followed by one byte = milliseconds to wait.
	static constexpr uint8_t InitSeqDelay = 0xFF;
	std::span<const uint8_t> init_seq{};

	// ---- Derived values ----------------------------------------------------------
	constexpr uint32_t h_total() const {
		return h_active + h_front + h_sync + h_back;
	}
	constexpr uint32_t v_total() const {
		return v_active + v_front + v_sync + v_back;
	}
	constexpr uint32_t lane_byte_clk_hz() const {
		return lane_rate_mbps * 125'000; // rate * 1e6 / 8
	}
	// Max pixel rate the link can carry at 24 bpp
	constexpr uint32_t dsi_pixel_throughput_hz() const {
		return static_cast<uint32_t>(uint64_t(lane_rate_mbps) * 1'000'000 * num_lanes / 24);
	}
	constexpr uint32_t pll_data_rate_mbps() const {
		return (2 * (pll_ref_khz / 1000) * pll_ndiv) / (pll_idf * (1u << pll_odf));
	}
	constexpr uint32_t num_init_writes() const {
		uint32_t n = 0;
		for (size_t i = 0; i < init_seq.size();) {
			if (init_seq[i] == InitSeqDelay) {
				i += 2;
			} else {
				i += 1 + init_seq[i];
				n++;
			}
		}
		return n;
	}

	// Sanity checks for static_assert() in panel definitions
	constexpr bool init_seq_well_formed() const {
		size_t i = 0;
		while (i < init_seq.size()) {
			if (init_seq[i] == 0)
				return false;
			i += (init_seq[i] == InitSeqDelay) ? 2 : 1 + init_seq[i];
		}
		return i == init_seq.size();
	}

	constexpr bool is_valid() const {
		return h_active > 0 && v_active > 0 && num_lanes >= 1 && num_lanes <= 4 && pll_ref_khz >= 17'000 &&
			   pll_ref_khz <= 27'000 && pll_idf >= 1 && pll_idf <= 16 && pll_ndiv >= 64 && pll_ndiv <= 625 &&
			   pll_odf <= 3 && pll_data_rate_mbps() == lane_rate_mbps && pixel_clk_max_hz > 0 &&
			   pixel_clk_max_hz < dsi_pixel_throughput_hz() && tx_escape_clk_div > 0 &&
			   lane_byte_clk_hz() / tx_escape_clk_div <= 20'000'000 && init_seq_well_formed();
	}
};

} // namespace mdrivlib
