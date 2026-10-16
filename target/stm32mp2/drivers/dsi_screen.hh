#pragma once
#include "drivers/callable.hh"
#include "drivers/dsi_panel.hh"
#include "drivers/pin.hh"
#include "stm32mp2xx_hal.h"
#include <cstdint>
#include <span>

// MIPI-DSI + LTDC display driver for the STM32MP25x (Cortex-A35, EL3 secure).
//
// Pipeline:  framebuffer (DDR) -> LTDC layer 1 -> DPI -> DSI host -> D-PHY -> panel
//
// Ported from the stm32mp2-baremetal ltdc-dsi/ (ILI9881C, 720x1280, 4 lanes) and
// ltdc-dsi-028/ (ST7701S, 480x480, 2 lanes) examples, both verified on hardware.
// Panel-specific data is a DsiPanel (drivers/dsi_panel.hh, drivers/panels/*.hh);
// board-specific data is a DsiScreenConf (below).
//
// Requirements (the app must provide):
//  - SystemA35_SYSTICK_Config() has run (stm32mp2_ca35/drivers/hal_cnt.cc): the
//    ST HAL uses HAL_GetTick()/HAL_Delay() for DSI PLL lock and FIFO timeouts.
//  - HAL DSI + LTDC modules enabled in stm32mp2xx_hal_conf.h and their .c files
//    built. HAL_DSI_MspInit/HAL_LTDC_MspInit are expected to be the weak no-ops:
//    this driver does the clocks/resets itself.
//  - HSE running (TF-A does this). For the best refresh rate PLL4 should be
//    running from HSE (TF-A sets PLL4 = 1200 MHz on the 4ms boards); otherwise the
//    pixel clock falls back to HSE/N.
//  - The GIC is initialized and IRQs are unmasked if you want vblank pacing
//    (wait_vblank(), the vblank callback). Page flips latch in hardware either way.
//  - The framebuffer is in memory the LTDC can read (DDR) and is cache-clean when
//    scanned out: either map it non-cacheable/write-through, or call
//    clean_framebuffer_cache() after drawing. Address must fit in 32 bits.
//
// The driver does (init()):
//  1. RIF: RISUP 80 (LTDC), 81 (DSI), 119 (LTDC layers) secure, and RIMU 11 (LTDC
//     DMA master) MSEC|MPRIV so LTDC reads pass the RISAF DDR firewall.
//  2. Clocks: flexgen ch28 ck_ker_dsiphy = HSE/2 (D-PHY PLL ref), flexgen ch27
//     ck_ker_ltdc = PLL4/N or HSE/N (pixel clock), LTDC + DSI bus clocks + reset
//     pulse, RCC_DSICFGR CKREFSEL/BLSEL = 0, SYSCFG DISPLAYCLKCR = 0b10.
//  3. DSI: D-PHY PLL lock + PHY up (HAL_DSI_Init), LP command mode, PHY timers.
//  4. Panel: hardware reset pulse, init sequence as LP DCS writes.
//  5. LTDC: timings, layer 1 (with the CFBLR pitch fix), LINE IRQ at vblank.
//  6. DSI video mode, burst, LP in blanking; start the stream.
//  7. Backlight on.
//
// Gotchas preserved from the PoC (see dsi_screen.cc for details):
//  - The vendored HAL's LTDC_SetConfig writes a NEGATIVE pitch to CFBLR; the
//    driver rewrites CFBLR by hand after HAL_LTDC_ConfigLayer.
//  - MP25 layer shadow registers latch via the per-layer reload register (LxRCR);
//    the reload-done IRQ (IT_RR) never fires for per-layer reloads, so flips are
//    paced off the LINE interrupt.
//  - LTDC pixel clock must be HSE-derived (phase-locked to the DSI byte clock) or
//    the colours cycle, and below the DSI throughput or burst mode has no room.

namespace mdrivlib
{

struct DsiScreenConf {
	DsiPanel panel{};

	// Panel reset GPIO. GPIO::Unused = no reset pin (panel reset is skipped).
	PinDef reset_pin{};
	bool reset_active_low = true;

	// Backlight enable GPIO (e.g. boost LED driver enable). GPIO::Unused = none.
	PinDef backlight_pin{};
	bool backlight_active_high = true;

	// HSE frequency. ck_ker_dsiphy is HSE/2, which must equal panel.pll_ref_khz.
	uint32_t hse_hz = 40'000'000;

	// LTDC layer pixel format in memory. The DSI link is always RGB888.
	// Only ARGB8888 has been verified on hardware.
	enum class PixelFormat : uint8_t { ARGB8888, RGB888, RGB565 };
	PixelFormat pixel_format = PixelFormat::ARGB8888;

	// Layer window within the active area. w/h = 0 means full screen.
	// background_rgb (0xRRGGBB) fills the area outside the window.
	uint32_t layer_x = 0;
	uint32_t layer_y = 0;
	uint32_t layer_w = 0;
	uint32_t layer_h = 0;
	uint32_t background_rgb = 0;

	// LTDC LINE (vblank) IRQ priority, see InterruptControl::set_irq_priority
	unsigned irq_pri1 = 0;
	unsigned irq_pri2 = 0;
};

class DsiScreen {
public:
	enum class Error {
		None,
		InvalidPanel,		 // DsiPanel::is_valid() failed
		PhyRefClockMismatch, // hse_hz/2 != panel.pll_ref_khz
		DsiPllLockTimeout,	 // HAL_DSI_Init failed: D-PHY PLL never locked / lanes never reached stop-state
	};

	DsiScreen(const DsiScreenConf &conf);

	// Full bring-up (see header comment). Call once. framebuffer = first buffer to scan out.
	Error init(uint32_t framebuffer_addr);
	Error init(const void *framebuffer) {
		return init(addr32(framebuffer));
	}

	// Tear-free page flip: point layer 1 at a new framebuffer, latched by hardware
	// at the next vblank. Use wait_vblank() to pace one flip per refresh.
	void set_framebuffer(uint32_t framebuffer_addr);
	void set_framebuffer(const void *framebuffer) {
		set_framebuffer(addr32(framebuffer));
	}

	// Block (in WFE) until the next vblank LINE IRQ. The flip armed by
	// set_framebuffer() latches at that same vblank. Returns false on timeout
	// (IRQ never arrived). Requires IRQs unmasked.
	bool wait_vblank(uint32_t timeout_us = 25'000);

	// Called from the LTDC IRQ at the start of every vblank.
	void set_vblank_callback(Callback &&cb);

	void backlight_on();
	void backlight_off();

	// Gate the LTDC scan-out (GCR.LTDCEN).
	void enable_scanout(bool on);

	// (Re)start the DSI video stream with the LTDC gated off around it, so the
	// stream always begins on a clean frame with an empty DPI FIFO. Use this for
	// any restart after init().
	void restart_video();

	// Clean the data cache over one framebuffer (width*height*bpp bytes) so the
	// LTDC sees what the CPU drew. Not needed if the buffer is mapped non-cacheable.
	void clean_framebuffer_cache(const void *framebuffer) const;

	// Send one DCS write in LP (len 1: short no-param, 2: short 1-param, >2: long).
	bool dcs_write(std::span<const uint8_t> data);

	// DCS read in LP over D0 (bus turnaround). Requires panel.enable_bta.
	// Returns false on timeout / no answer.
	bool dcs_read(uint8_t cmd, uint8_t *buf, uint32_t len);

	// ---- Info / diagnostics ----
	uint32_t width() const;
	uint32_t height() const;
	uint32_t bytes_per_pixel() const {
		return bytes_per_pixel(conf_.pixel_format);
	}
	uint32_t framebuffer_bytes() const {
		return width() * height() * bytes_per_pixel();
	}
	uint32_t pixel_clock_hz() const {
		return pixel_clk_hz_;
	}
	uint32_t refresh_rate_millihz() const;
	uint32_t vblank_count() const {
		return vblank_count_;
	}
	uint32_t dcs_write_errors() const {
		return dcs_write_errors_;
	}
	uint32_t current_line() const;	  // LTDC CPSR.CYPOS: advancing == pixel clock alive
	uint32_t ltdc_isr() const;		  // bit1 FIFO warn, bit2 transfer err, bit6 FIFO err
	uint32_t ltdc_hw_version() const; // expect 0x0401xx on MP25
	uint32_t dsi_hw_version() const;
	uint32_t dsi_isr0() const; // bits0-15 ACK errors reported by the panel, 16-20 PHY errors
	uint32_t dsi_isr1() const; // bit0 HS-TX timeout, 1 LP-RX timeout, 2/3 ECC, 4 CRC, 5 pkt size, 6 EOTP, 7+ FIFO
	uint32_t dsi_psr() const;  // PHY status (lane stop-states, ULPS, RX-active)

	static constexpr uint32_t bytes_per_pixel(DsiScreenConf::PixelFormat pf) {
		return pf == DsiScreenConf::PixelFormat::RGB565 ? 2 : pf == DsiScreenConf::PixelFormat::RGB888 ? 3 : 4;
	}

private:
	DsiScreenConf conf_;
	DSI_HandleTypeDef hdsi_{};
	LTDC_HandleTypeDef hltdc_{};
	uint32_t pixel_clk_hz_ = 0;
	uint32_t layer_w_ = 0;
	uint32_t layer_h_ = 0;
	volatile uint32_t vblank_count_ = 0;
	uint32_t dcs_write_errors_ = 0;
	Callback vblank_cb_;

	void rif_setup();
	uint32_t clocks_setup();
	bool dsi_pll_init();
	void dsi_command_mode();
	void dsi_video_config();
	void panel_reset();
	void send_init_seq();
	void ltdc_init(uint32_t fb_addr);
	void ltdc_isr_handler();
	void set_pin(const PinDef &pin, bool level);

	static uint32_t addr32(const void *p) {
		return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(p));
	}
};

} // namespace mdrivlib
