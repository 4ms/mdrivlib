#include "drivers/dsi_screen.hh"
#include "drivers/cache.hh"
#include "drivers/interrupt.hh"
#include "drivers/interrupt_control.hh"
#include "drivers/rcc_common.hh"
#include "drivers/rcc_pll.hh"
#include "drivers/rcc_xbar.hh"
#include "drivers/system_reg.hh"

// =============================================================================
//  MP25 MIPI-DSI host + Synopsys D-PHY + LTDC, via ST's HAL drivers
// =============================================================================
// Ported from stm32mp2-baremetal ltdc-dsi/ (ILI9881C) and ltdc-dsi-028/
// (ST7701S), both verified on hardware. The DSI and LTDC are configured through
// ST's stm32mp2xx_hal_dsi.c / stm32mp2xx_hal_ltdc.c so the bring-up reads like
// ST's own examples. Things done by hand are commented as such.
//
// LTDC, two things done by hand, deliberately:
//
//  1. CFBLR pitch. This vendored HAL's LTDC_SetConfig() (used by ConfigLayer and
//     SetAddress) writes CFBP = 0x10000 - ImageWidth*stride -- a *negative* pitch
//     copied from the rotation/AFBLR path. The register is a plain byte pitch
//     (HAL_LTDC_SetPitch and the mirror helpers write the raw ImageWidth*stride
//     to the same field), and the raw pitch is what actually scans out on this
//     silicon. So after ConfigLayer we rewrite CFBLR with the raw pitch and
//     re-latch. The vendored HAL is left untouched.
//  2. The vblank flip is a 2-register hot path (CFBAR + per-layer VBR reload) --
//     using HAL_LTDC_SetAddress would re-run the buggy LTDC_SetConfig every frame.
//
// MP25 note: layer shadow registers latch via the *per-layer* reload register
// (LxRCR: IMR bit0 immediate, VBR bit1 at next vblank); the reload-done IRQ
// (IT_RR) never fires for per-layer reloads, so the flip is paced off the LINE
// interrupt instead.

namespace mdrivlib
{

namespace
{

// RIFSC RISUP peripheral ids, to expose the register ports to our secure CA35
// context (LVDS = 84, unused here). DSI = 81, confirmed: RIF_PERIPH_DSI_CMN =
// REG2 | SEC81 (stm32mp2xx_hal_rif.h).
constexpr uint32_t RifscId_LTDC = 80;
constexpr uint32_t RifscId_DSI = 81;
constexpr uint32_t RifscId_LTDC_Layers = 119;
// RIMU index of the LTDC_L1/L2 DMA master
constexpr uint32_t RimuId_LTDC = 11;

// SYSCFG DISPLAYCLKCR.PIXEL_CLK_SEL -- LTDC pixel-clock source (RM0457):
//   0b00 = DSI clock from the DSI PLL   (loopback; pixel clock locked to the PHY)
//   0b01 = LVDS clock from the LVDS PLL
//   0b10 = ck_ker_ltdc (flexgen ch27)   (independent, for parallel or DSI)
// We drive ck_ker_ltdc independently (below the DSI's pixel throughput) so the
// DSI link has bandwidth headroom -- the loopback (0b00) locks pixel_clk to the
// lane rate exactly (zero headroom), which forced LP on every line and wrapped
// the image.
constexpr uint32_t PixelClkSel_CkKerLtdc = 0b10;

// LTDC AXI bus width in bytes (MP25 = 64-bit). CFBLR line length = bytes + width - 1
constexpr uint32_t LtdcBusWidthBytes = 8;

// Busy-wait on the generic timer (doesn't depend on SystemA35_SYSTICK_Config).
void delay_us(uint32_t us) {
	const uint64_t start = read_cntpct();
	const uint64_t ticks = read_cntfreq() * us / 1'000'000;
	while ((read_cntpct() - start) < ticks)
		;
}

uint32_t ltdc_pixel_format(DsiScreenConf::PixelFormat pf) {
	switch (pf) {
		case DsiScreenConf::PixelFormat::RGB565:
			return LTDC_PIXEL_FORMAT_RGB565;
		case DsiScreenConf::PixelFormat::RGB888:
			return LTDC_PIXEL_FORMAT_RGB888;
		case DsiScreenConf::PixelFormat::ARGB8888:
		default:
			return LTDC_PIXEL_FORMAT_ARGB8888;
	}
}

// ceil(f / max), clamped to the flexgen FINDIV range 1..64
uint32_t findiv_for(uint32_t f, uint32_t max) {
	uint32_t div = static_cast<uint32_t>((uint64_t(f) + max - 1) / max);
	return div < 1 ? 1 : div > 64 ? 64 : div;
}

} // namespace

DsiScreen::DsiScreen(const DsiScreenConf &conf)
	: conf_{conf} {
	layer_w_ = conf_.layer_w ? conf_.layer_w : conf_.panel.h_active;
	layer_h_ = conf_.layer_h ? conf_.layer_h : conf_.panel.v_active;
}

DsiScreen::Error DsiScreen::init(uint32_t framebuffer_addr) {
	if (!conf_.panel.is_valid())
		return Error::InvalidPanel;

	if (conf_.hse_hz / 2 != conf_.panel.pll_ref_khz * 1000)
		return Error::PhyRefClockMismatch;

	rif_setup();
	pixel_clk_hz_ = clocks_setup();

	if (!dsi_pll_init())
		return Error::DsiPllLockTimeout;

	dsi_command_mode();
	panel_reset();
	send_init_seq();

	ltdc_init(framebuffer_addr);

	if (conf_.panel.gated_video_start) {
		restart_video();
	} else {
		// The ILI9881C bring-up as verified: video config written into the host
		// that is already enabled for LP commands, with the LTDC already scanning.
		dsi_video_config();
		HAL_DSI_Start(&hdsi_);
	}

	backlight_on();
	return Error::None;
}

// RIF: expose the LTDC + DSI register ports to our context and make the LTDC's
// DMA reads secure so they pass the RISAF DDR firewall. The DSI is not a DMA
// master (the LTDC feeds it pixels over DPI), so it needs no RIMU entry.
void DsiScreen::rif_setup() {
	RISC->SECCFGR[RifscId_LTDC / 32] |= (1u << (RifscId_LTDC % 32));
	RISC->SECCFGR[RifscId_DSI / 32] |= (1u << (RifscId_DSI % 32));
	RISC->SECCFGR[RifscId_LTDC_Layers / 32] |= (1u << (RifscId_LTDC_Layers % 32));

	auto attr = RIMC->ATTR[RimuId_LTDC];
	RIMC->ATTR[RimuId_LTDC] = (attr & ~RIMC_ATTR_CIDSEL) | RIMC_ATTR_MSEC | RIMC_ATTR_MPRIV;
}

// LTDC/DSI gates + resets, ck_ker_dsiphy (HSE/2), and the LTDC pixel clock
// (ck_ker_ltdc). Returns the actual LTDC pixel-clock frequency in Hz.
uint32_t DsiScreen::clocks_setup() {
	using namespace RCC_Clocks;

	// --- ck_ker_dsiphy (flexgen 28) = HSE / 2 (= 20 MHz with a 40 MHz HSE) ------
	// This is the D-PHY PLL reference (17-27 MHz).
	FlexbarConf{.PLL = FlexbarConf::PLLx::HSE, .findiv = 1, .prediv = 0}.init(FlexbarConf::DSIPHY_);

	// --- ck_ker_ltdc (flexgen 27) = the LTDC pixel clock -------------------------
	// Must be derived from the same clock as the DSI byte clock (HSE) so they're
	// phase-locked, otherwise the colors cycle. And must be below the DSI pixel
	// throughput so burst mode has headroom.
	// Use PLL4 if it runs from HSE: the largest PLL4/N <= panel.pixel_clk_max_hz
	// (PLL4 = 1200 MHz -> /18 = 66.7 MHz for the ILI9881C panel). Otherwise fall
	// back to HSE/N (locked, but a lower refresh rate).
	const uint32_t pix_max = conf_.panel.pixel_clk_max_hz;
	const auto pll4_settings = get_pll_settings<PLL4>();
	const bool pll4_from_hse = pll4_settings.src == MuxSelSource::hse && PLL4::Ready::read();
	const uint32_t pll4_hz = pll4_from_hse ? pll4_settings.calc_freq(conf_.hse_hz) : 0;

	uint32_t pix;
	if (pll4_hz >= pix_max) {
		auto findiv = findiv_for(pll4_hz, pix_max);
		FlexbarConf{.PLL = FlexbarConf::PLLx::_4, .findiv = static_cast<uint8_t>(findiv - 1), .prediv = 0}.init(
			FlexbarConf::LTDC_);
		pix = pll4_hz / findiv;
	} else {
		auto findiv = findiv_for(conf_.hse_hz, pix_max);
		FlexbarConf{.PLL = FlexbarConf::PLLx::HSE, .findiv = static_cast<uint8_t>(findiv - 1), .prediv = 0}.init(
			FlexbarConf::LTDC_);
		pix = conf_.hse_hz / findiv;
	}

	// --- bus/kernel clock gates (single gate per IP on MP25) ---------------------
	RCC_Enable::LTDC_::set();
	RCC_Enable::DSI_::set();

	// --- pulse resets -----------------------------------------------------------
	RCC_Reset::LTDC_::set();
	RCC_Reset::DSI_::set();
	delay_us(20);
	RCC_Reset::LTDC_::clear();
	RCC_Reset::DSI_::clear();

	// --- DSI clock-source muxes (RCC_DSICFGR) -------------------------------------
	RCC->DSICFGR &= ~RCC_DSICFGR_DSIPHYCKREFSEL; // PHY PLL ref = ck_ker_dsiphy
	RCC->DSICFGR &= ~RCC_DSICFGR_DSIBLSEL;		 // byte-lane clock = DSI-PHY

	// --- LTDC pixel clock source = ck_ker_ltdc --------------------------------------
	SYSCFG->DISPLAYCLKCR = PixelClkSel_CkKerLtdc;

	return pix;
}

// Configure + lock the D-PHY PLL, power up the PHY, and wait for all lanes to
// reach stop-state. HAL_DSI_MspInit() must be a no-op: the DSI bus clock/reset
// and the PHY reference clock are already set up by clocks_setup().
bool DsiScreen::dsi_pll_init() {
	const auto &p = conf_.panel;

	hdsi_.Instance = DSI;
	hdsi_.Init.AutomaticClockLaneControl = DSI_AUTO_CLK_LANE_CTRL_DISABLE; // continuous HS clock
	hdsi_.Init.TXEscapeCkdiv = p.tx_escape_clk_div;
	hdsi_.Init.NumberOfLanes = p.num_lanes - 1; // DSI_ONE_DATA_LANE = 0 ... DSI_FOUR_DATA_LANES = 3
	// PhyDataRate selects the row of ST's D-PHY regulation table (vco/prop/hs_freq)
	// used for the PLL tuning. pll_odf must match that row's ODF column.
	hdsi_.Init.PhyDataRate = p.phy_data_rate;

	DSI_PLLInitTypeDef pll{};
	pll.WrapPHYFrequency = 0; // unused by the driver
	pll.RefClkin = p.pll_ref_khz;
	pll.PLLNDIV = p.pll_ndiv;
	pll.PLLIDF = p.pll_idf;
	pll.PLLODF = p.pll_odf;

	// HAL_DSI_Init: PLL tuning + lock, PHY power-up, wait for all lanes in stop-state.
	return HAL_DSI_Init(&hdsi_, &pll) == HAL_OK;
}

// Program the host for LP command transmission (used to push the panel init
// sequence before the video stream starts).
void DsiScreen::dsi_command_mode() {
	// Send every command class in low power (the panel init goes out in LP before
	// the video stream starts).
	DSI_LPCmdTypeDef lp{};
	lp.LPGenShortWriteNoP = DSI_LP_GSW0P_ENABLE;
	lp.LPGenShortWriteOneP = DSI_LP_GSW1P_ENABLE;
	lp.LPGenShortWriteTwoP = DSI_LP_GSW2P_ENABLE;
	lp.LPGenShortReadNoP = DSI_LP_GSR0P_ENABLE;
	lp.LPGenShortReadOneP = DSI_LP_GSR1P_ENABLE;
	lp.LPGenShortReadTwoP = DSI_LP_GSR2P_ENABLE;
	lp.LPGenLongWrite = DSI_LP_GLW_ENABLE;
	lp.LPDcsShortWriteNoP = DSI_LP_DSW0P_ENABLE;
	lp.LPDcsShortWriteOneP = DSI_LP_DSW1P_ENABLE;
	lp.LPDcsShortReadNoP = DSI_LP_DSR0P_ENABLE;
	lp.LPDcsLongWrite = DSI_LP_DLW_ENABLE;
	lp.LPMaxReadPacket = DSI_LP_MRDP_ENABLE;
	lp.AcknowledgeRequest = DSI_ACKNOWLEDGE_DISABLE;
	HAL_DSI_ConfigCommand(&hdsi_, &lp);

	// Bus turnaround so the panel can drive D0 back to us for DCS reads, plus
	// ECC/CRC checking on what it returns (Linux sets the same three bits).
	if (conf_.panel.enable_bta)
		HAL_DSI_ConfigFlowControl(&hdsi_, DSI_FLOW_CONTROL_BTA | DSI_FLOW_CONTROL_ECC_RX | DSI_FLOW_CONTROL_CRC_RX);

	// PHY LP<->HS transition timers (lane-byte-clock cycles), from the panel table.
	DSI_PHY_TimerTypeDef t{};
	t.ClockLaneHS2LPTime = conf_.panel.phy_timers.clk_hs2lp;
	t.ClockLaneLP2HSTime = conf_.panel.phy_timers.clk_lp2hs;
	t.DataLaneHS2LPTime = conf_.panel.phy_timers.data_hs2lp;
	t.DataLaneLP2HSTime = conf_.panel.phy_timers.data_lp2hs;
	t.DataLaneMaxReadTime = 0;
	t.StopWaitTime = conf_.panel.phy_timers.stop_wait;
	HAL_DSI_ConfigPhyTimer(&hdsi_, &t);

	// Enable the host (not the wrapper yet) so the LP DCS init writes transmit.
	__HAL_DSI_ENABLE(&hdsi_);
}

// Program video-mode timings, DPI color coding (RGB888) and LP flags. Does not
// start the stream (HAL_DSI_Start does).
void DsiScreen::dsi_video_config() {
	const auto &p = conf_.panel;

	// Horizontal DSI timings are in lane-byte-clock cycles = ceil(pixels * byte_clk
	// / pixel_clk). pixel_clk is the runtime (PLL4- or HSE-derived) LTDC clock.
	// Vertical timings are in lines.
	const uint64_t byte_clk = p.lane_byte_clk_hz();
	const uint64_t pix_clk = pixel_clk_hz_;
	auto lbcc = [=](uint32_t px) { return static_cast<uint32_t>((px * byte_clk + (pix_clk - 1)) / pix_clk); };

	DSI_VidCfgTypeDef v{};
	v.VirtualChannelID = 0;
	v.ColorCoding = DSI_RGB888;
	v.LooselyPacked = DSI_LOOSELY_PACKED_DISABLE;
	v.Mode = p.video_mode;
	v.PacketSize = p.h_active;
	v.NumberOfChunks = 0;
	v.NullPacketSize = 0;
	v.HSPolarity = DSI_HSYNC_ACTIVE_HIGH;
	v.VSPolarity = DSI_VSYNC_ACTIVE_HIGH;
	v.DEPolarity = DSI_DATA_ENABLE_ACTIVE_HIGH;
	v.HorizontalSyncActive = lbcc(p.h_sync);
	v.HorizontalBackPorch = lbcc(p.h_back);
	v.HorizontalLine = lbcc(p.h_total());
	v.VerticalSyncActive = p.v_sync;
	v.VerticalBackPorch = static_cast<uint32_t>(int32_t(p.v_back) + p.dsi_vbp_offset);
	v.VerticalFrontPorch = static_cast<uint32_t>(int32_t(p.v_front) - p.dsi_vbp_offset);
	v.VerticalActive = p.v_active;

	// LP during vertical blanking is always on: the ILI9881C needs it as its frame
	// delimiter (holding vblank in HS blanks the panel).
	v.LPCommandEnable = DSI_LP_COMMAND_ENABLE;
	v.LPVerticalSyncActiveEnable = DSI_LP_VSYNC_ENABLE;
	v.LPVerticalBackPorchEnable = DSI_LP_VBP_ENABLE;
	v.LPVerticalFrontPorchEnable = DSI_LP_VFP_ENABLE;
	// Active-line horizontal-porch LP (panel option, both verified panels need it on;
	// the ST7701S DPI FIFO overflows every line without it).
	v.LPHorizontalFrontPorchEnable = p.lp_during_active_hblank ? DSI_LP_HFP_ENABLE : DSI_LP_HFP_DISABLE;
	v.LPHorizontalBackPorchEnable = p.lp_during_active_hblank ? DSI_LP_HBP_ENABLE : DSI_LP_HBP_DISABLE;
	v.LPVerticalActiveEnable = p.lp_during_active_hblank ? DSI_LP_VACT_ENABLE : DSI_LP_VACT_DISABLE;
	v.LPLargestPacketSize = p.lp_largest_packet;
	v.LPVACTLargestPacketSize = p.lp_vact_largest_packet;
	v.FrameBTAAcknowledgeEnable = DSI_FBTAA_DISABLE;
	HAL_DSI_ConfigVideoMode(&hdsi_, &v);
}

void DsiScreen::restart_video() {
	// The DSI stream is only ever (re)started with the LTDC gated off: starting
	// the host while the LTDC is already scanning leaves a partial pixel in the
	// host's DPI FIFO and the whole stream comes out offset by 1-2 bytes (red
	// shows as green, image wrapped), differently on every restart.
	enable_scanout(false);

	// The host must be disabled while its mode/video config is written: writing it
	// into a host already enabled for LP commands left the stream blank until the
	// host was stop/started. HAL_DSI_Stop clears PWR_UP + the wrapper.
	HAL_DSI_Stop(&hdsi_);
	dsi_command_mode();
	HAL_DSI_Stop(&hdsi_);
	dsi_video_config();

	// Enable the host + wrapper, so LTDC DPI pixels get packetised onto the link.
	HAL_DSI_Start(&hdsi_);
	delay_us(1000); // host + wrapper up and waiting for DPI VSYNC
	enable_scanout(true);
}

bool DsiScreen::dcs_write(std::span<const uint8_t> data) {
	HAL_StatusTypeDef res;
	if (data.size() == 0)
		return false;
	else if (data.size() <= 2)
		res = HAL_DSI_ShortWrite(&hdsi_,
								 0,
								 (data.size() == 1) ? DSI_DCS_SHORT_PKT_WRITE_P0 : DSI_DCS_SHORT_PKT_WRITE_P1,
								 data[0],
								 (data.size() == 2) ? data[1] : 0);
	else
		// Param1 = DCS command byte, the remaining len-1 bytes are parameters.
		res = HAL_DSI_LongWrite(&hdsi_, 0, DSI_DCS_LONG_PKT_WRITE, data.size() - 1, data[0], &data[1]);

	return res == HAL_OK;
}

bool DsiScreen::dcs_read(uint8_t cmd, uint8_t *buf, uint32_t len) {
	if (!conf_.panel.enable_bta)
		return false;

	// HAL_DSI_Read only programs the max-return-packet-size for len > 2, and the
	// host's reset default is not guaranteed to be what we need, so always send
	// it: DT 0x37 (Set Maximum Return Packet Size), payload = len.
	uint32_t t0 = HAL_GetTick();
	while ((DSI->GPSR & DSI_GPSR_CMDFE) == 0) {
		if (HAL_GetTick() - t0 > 100)
			return false;
	}
	DSI->GHCR = 0x37u | (0u << 6) | ((len & 0xFF) << 8) | (((len >> 8) & 0xFF) << 16);

	return HAL_DSI_Read(&hdsi_, 0, buf, len, DSI_DCS_SHORT_PKT_READ, cmd, nullptr) == HAL_OK;
}

void DsiScreen::set_pin(const PinDef &pin, bool level) {
	if (pin.gpio == GPIO::Unused)
		return;
	Pin p{pin, PinMode::Output};
	if (level)
		p.high();
	else
		p.low();
}

void DsiScreen::panel_reset() {
	if (conf_.reset_pin.gpio == GPIO::Unused)
		return;

	const bool asserted = !conf_.reset_active_low;
	const auto &p = conf_.panel;

	if (p.reset_pre_deassert_us) {
		set_pin(conf_.reset_pin, !asserted);
		delay_us(p.reset_pre_deassert_us);
	}
	set_pin(conf_.reset_pin, asserted);
	delay_us(p.reset_assert_us);
	set_pin(conf_.reset_pin, !asserted);
	delay_us(p.reset_settle_us);
}

// Walk the panel init table and push every record to the panel as a DCS write
// in LP mode, honouring the embedded delays. Requires the DSI PLL up and the
// panel already hardware-reset. Sent BEFORE starting the video stream.
void DsiScreen::send_init_seq() {
	auto seq = conf_.panel.init_seq;
	size_t i = 0;
	while (i < seq.size()) {
		uint8_t len = seq[i++];
		if (len == DsiPanel::InitSeqDelay) {
			if (i < seq.size())
				delay_us(1000u * seq[i++]);
			continue;
		}
		if (i + len > seq.size())
			break;
		if (!dcs_write(seq.subspan(i, len)))
			dcs_write_errors_ = dcs_write_errors_ + 1;
		i += len;
	}
}

// Program timings + one layer scanning out `fb_addr`, latch it, enable the
// controller, and hook up the vblank (LINE) interrupt. The pixel clock
// (ck_ker_ltdc via the SYSCFG mux) must be ticking first.
void DsiScreen::ltdc_init(uint32_t fb_addr) {
	const auto &p = conf_.panel;

	// DRM-style mode points -> LTDC accumulated-timing register fields, with the
	// same formulas Linux drivers/gpu/drm/stm/ltdc.c uses
	const uint32_t hsync_start = p.h_active + p.h_front;
	const uint32_t hsync_end = hsync_start + p.h_sync;
	const uint32_t htotal = hsync_end + p.h_back;
	const uint32_t vsync_start = p.v_active + p.v_front;
	const uint32_t vsync_end = vsync_start + p.v_sync;
	const uint32_t vtotal = vsync_end + p.v_back;

	const uint32_t accum_hbp = htotal - hsync_start - 1;
	const uint32_t accum_vbp = vtotal - vsync_start - 1;
	const uint32_t accum_act_h = accum_vbp + p.v_active;

	// No active-edge overrides (the "AL"/"IPC" constants are all 0, i.e. a zeroed
	// GCR polarity field) -- the syncs are embedded in the DSI stream anyway.
	hltdc_.Instance = LTDC;
	hltdc_.Init.HSPolarity = LTDC_HSPOLARITY_AL;
	hltdc_.Init.VSPolarity = LTDC_VSPOLARITY_AL;
	hltdc_.Init.DEPolarity = LTDC_DEPOLARITY_AL;
	hltdc_.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
	hltdc_.Init.HorizontalSync = hsync_end - hsync_start - 1;
	hltdc_.Init.VerticalSync = vsync_end - vsync_start - 1;
	hltdc_.Init.AccumulatedHBP = accum_hbp;
	hltdc_.Init.AccumulatedVBP = accum_vbp;
	hltdc_.Init.AccumulatedActiveW = accum_hbp + p.h_active;
	hltdc_.Init.AccumulatedActiveH = accum_act_h;
	hltdc_.Init.TotalWidth = htotal - 1;
	hltdc_.Init.TotalHeigh = vtotal - 1;
	hltdc_.Init.Backcolor.Red = (conf_.background_rgb >> 16) & 0xFF;
	hltdc_.Init.Backcolor.Green = (conf_.background_rgb >> 8) & 0xFF;
	hltdc_.Init.Backcolor.Blue = conf_.background_rgb & 0xFF;
	hltdc_.Init.FifoUnderThresh = 0x80; // driver-default underrun warning threshold
	HAL_LTDC_Init(&hltdc_);				// programs GCR/timings + enables LTDCEN

	// Quiet interrupts: HAL_LTDC_Init turns on TE/FU (and the secure copies); we
	// only want the LINE interrupt (fires once per frame at start of vblank).
	LTDC->IER = LTDC_IER_LIE;
	LTDC->IER2 = 0; // secure IER off
	LTDC->ICR = 0x3F;
	LTDC->LIPCR = accum_act_h + 1; // line IRQ at start of vblank

	// Layer 1: a w x h window at (x, y); HAL adds the accumulated back-porch
	// offsets internally, so pass raw positions.
	const uint32_t w = layer_w_;
	const uint32_t h = layer_h_;
	const uint32_t bpp = bytes_per_pixel();

	LTDC_LayerCfgTypeDef lc{};
	lc.WindowX0 = conf_.layer_x;
	lc.WindowX1 = conf_.layer_x + w;
	lc.WindowY0 = conf_.layer_y;
	lc.WindowY1 = conf_.layer_y + h;
	lc.PixelFormat = ltdc_pixel_format(conf_.pixel_format);
	lc.Alpha = 0xFF; // constant alpha 1.0
	lc.Alpha0 = 0;
	lc.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
	lc.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
	lc.FBStartAdress = fb_addr;
	lc.ImageWidth = w;
	lc.ImageHeight = h;
	lc.BurstLength = 0; // max burst
	HAL_LTDC_ConfigLayer(&hltdc_, &lc, LTDC_LAYER_1);

	// Fix the CFBLR pitch the HAL got wrong (see file header), then re-latch it.
	// CFBLR = pitch<<16 | (line_bytes + bus_width_bytes - 1); 64-bit bus -> +7.
	LTDC_Layer1->CFBLR = ((w * bpp) << 16) | (w * bpp + LtdcBusWidthBytes - 1);
	LTDC_Layer1->RCR = LTDC_LxRCR_IMR; // per-layer immediate reload

	Interrupt::register_and_start_isr(LTDC_IRQn, conf_.irq_pri1, conf_.irq_pri2, [this] { ltdc_isr_handler(); });
}

void DsiScreen::ltdc_isr_handler() {
	uint32_t isr = LTDC->ISR;
	LTDC->ICR = isr; // write-1-to-clear everything we took
	if (isr & LTDC_ISR_LIF) {
		vblank_count_ = vblank_count_ + 1;
		if (vblank_cb_)
			vblank_cb_();
	}
}

void DsiScreen::set_framebuffer(uint32_t framebuffer_addr) {
	// Tear-free flip: new address + per-layer reload at the next vblank. Done by
	// hand (not HAL_LTDC_SetAddress) to avoid re-running the HAL's buggy pitch.
	LTDC_Layer1->CFBAR = framebuffer_addr;
	LTDC_Layer1->RCR = LTDC_LxRCR_VBR;
}

bool DsiScreen::wait_vblank(uint32_t timeout_us) {
	// Block until the next vblank (the LINE IRQ bumps vblank_count_ at the start
	// of every frame). The VBR flip we armed latches at that same vblank, so this
	// paces exactly one rendered+displayed frame per refresh, tear-free -- the CPU
	// sits in WFE meanwhile (exception return sets the event register).
	const uint32_t start = vblank_count_;
	const uint64_t deadline = read_cntpct() + read_cntfreq() * timeout_us / 1'000'000;
	while (vblank_count_ == start) {
		asm volatile("wfe");
		if (read_cntpct() > deadline)
			return false;
	}
	return true;
}

void DsiScreen::set_vblank_callback(Callback &&cb) {
	const bool was_enabled = InterruptControl::is_enabled_irq(LTDC_IRQn);
	InterruptControl::disable_irq(LTDC_IRQn);
	vblank_cb_ = std::move(cb);
	if (was_enabled)
		InterruptControl::reenable_irq(LTDC_IRQn);
}

void DsiScreen::backlight_on() {
	set_pin(conf_.backlight_pin, conf_.backlight_active_high);
}

void DsiScreen::backlight_off() {
	set_pin(conf_.backlight_pin, !conf_.backlight_active_high);
}

void DsiScreen::enable_scanout(bool on) {
	if (on)
		LTDC->GCR |= LTDC_GCR_LTDCEN;
	else
		LTDC->GCR &= ~LTDC_GCR_LTDCEN;
}

void DsiScreen::clean_framebuffer_cache(const void *framebuffer) const {
	SystemCache::clean_dcache_by_range(const_cast<void *>(framebuffer), framebuffer_bytes());
}

uint32_t DsiScreen::width() const {
	return layer_w_;
}

uint32_t DsiScreen::height() const {
	return layer_h_;
}

uint32_t DsiScreen::refresh_rate_millihz() const {
	const uint64_t frame_clocks = uint64_t(conf_.panel.h_total()) * conf_.panel.v_total();
	return frame_clocks ? static_cast<uint32_t>(uint64_t(pixel_clk_hz_) * 1000 / frame_clocks) : 0;
}

uint32_t DsiScreen::current_line() const {
	return LTDC->CPSR & 0xFFFF; // CYPOS
}

uint32_t DsiScreen::ltdc_isr() const {
	return LTDC->ISR;
}

uint32_t DsiScreen::ltdc_hw_version() const {
	return LTDC->IDR;
}

uint32_t DsiScreen::dsi_hw_version() const {
	return DSI->VERR;
}

uint32_t DsiScreen::dsi_isr0() const {
	return DSI->ISR0;
}

uint32_t DsiScreen::dsi_isr1() const {
	return DSI->ISR1;
}

uint32_t DsiScreen::dsi_psr() const {
	return DSI->PSR;
}

} // namespace mdrivlib
