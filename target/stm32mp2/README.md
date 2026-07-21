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

