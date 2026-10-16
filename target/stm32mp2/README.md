# STM32MP2 target

Support for the STM32MP25x family: 2x Cortex-A35 (AArch64) + Cortex-M33 (+ TODO: Cortex-M0+).

- `target/stm32mp2/` — drivers shared by both cores
- `target/stm32mp2_ca35/` — Cortex-A35 core: `boot/` (vectors, GIC init, page tables), `drivers/`, `templates/` (files to copy into your project and customize)
- `target/stm32mp2_cm33/` — Cortex-M33 core (placeholder, see its README)

Build with `-DSTM32MP2 -DCORE_CA35` (or `-DCORE_CM33`) and add `mdrivlib`, `mdrivlib/target/stm32mp2`, and `mdrivlib/target/stm32mp2_ca35` to the include path, per the top-level README.

## CMSIS device headers — use the patched pack

Do NOT use the stock ST CMSIS device headers:

1. ST's `POSITION_VAL` macro is broken on 64-bit targets. A patched `stm32mp2xx.h` is required.
2. The A35 device headers (`stm32mp2xxxxx_ca35.h`) are not in the Cube HAL release; they were sourced from ST's DDR-FW-UTIL repo.

The corrected pack lives in [stm32mp2-baremetal/shared/cmsis-device/](https://github.com/4ms/stm32mp2-baremetal/tree/main/shared/cmsis-device).
Copy it to your project and put it on the include path (mdrivlib includes it via `drivers/stm32xx.h`).

## Execution model

See [https://github.com/4ms/stm32mp2-baremetal](https://github.com/4ms/stm32mp2-baremetal) for the big picture, including how the TF-A bootloader works.

All ported code assumes the A35 runs in EL3 secure.

The IRQ entry path is `boot/vectors.S` (AArch64 vector table + GIC acknowledge/EOI)

The exception-dump path in `vectors.S` expects the app to provide `putchar_s()`
and `early_puthex64()`. See the stm32mp2-baremetal repo's `shared/print/uart_print.c`).
`boot/esr_decode.cc` is an optional ESR pretty-printer (weak-symbol override).


## MIPI-DSI + LTDC display (`drivers/dsi_screen.{hh,cc}`)

Framebuffer (DDR) -> LTDC layer 1 -> DSI host -> D-PHY -> panel, via ST's HAL DSI/LTDC
drivers. Ported from stm32mp2-baremetal `ltdc-dsi/` (ER-TFT050-10, ILI9881C, 720x1280,
4 lanes @ 450 Mbps) and `ltdc-dsi-028/` (ER-TFT028-2, ST7701S, 480x480, 2 lanes @ 400 Mbps),
both verified on hardware.

- `drivers/dsi_panel.hh`: `DsiPanel`, the panel definition (mode, lanes, D-PHY PLL, PHY
  timers, LP/video-mode flags, pixel-clock target, reset timing, init sequence).
- `drivers/panels/ili9881c.hh` (`Panels::ER_TFT050_10`), `drivers/panels/st7701s.hh`
  (`Panels::ER_TFT028_2`).
- `DsiScreenConf`: board data (reset/backlight `PinDef`s and polarity, HSE frequency,
  layer pixel format/window, IRQ priority). `DsiScreen{conf}.init(fb)` does everything:
  RIF, clocks, D-PHY PLL, panel reset + init sequence, LTDC, video stream, backlight.
  `set_framebuffer()` flips at the next vblank and `wait_vblank()` paces off the LTDC LINE IRQ.

Requires `SystemA35_SYSTICK_Config()` (HAL timeouts), HAL DSI+LTDC modules, HSE (and
ideally PLL4 from HSE, set by TF-A). The driver sets flexgen ch27/ch28 itself.

Gotchas (all handled in the driver, keep them if you touch it):
- The vendored HAL's `LTDC_SetConfig` writes a negative pitch (`0x10000 - w*bpp`) to
  `CFBLR`; the driver rewrites `CFBLR` by hand after `HAL_LTDC_ConfigLayer`, and flips
  with a raw `CFBAR` + `LxRCR.VBR` write instead of `HAL_LTDC_SetAddress`.
- MP25 per-layer reload (`LxRCR`) never raises `IT_RR`, so flips are paced off the LINE IRQ.
- The LTDC pixel clock must be HSE-derived (phase-locked to the DSI byte clock) or the
  colours cycle, and below the DSI throughput (lane rate x lanes / 24) or burst mode has no
  headroom. Don't use the DSI-PLL loopback (`DISPLAYCLKCR = 0b00`).
- ILI9881C needs LP during vertical blanking (always on). ST7701S needs burst + LP in the
  horizontal porches, the DSI started with the LTDC gated off, and the host stopped before
  the video config is written (`DsiPanel::gated_video_start`).
- RIF: `RISC->SECCFGR` bits for LTDC (80), DSI (81), LTDC layers (119), and
  `RIMC->ATTR[11]` MSEC|MPRIV so the LTDC's DMA reads pass the DDR firewall.
- The framebuffer must be cache-clean when scanned (`clean_framebuffer_cache()` or map it
  non-cacheable).
