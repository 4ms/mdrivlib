# STM32MP2 Cortex-M33 target

The M33 (CPU2) runs secure, with the SAU disabled, so all its bus accesses are secure
and it can use the same `0x4xxxxxxx` peripheral addresses as the A35. Build without
`CORTEX_IN_SECURE_STATE` so the CMSIS device header picks those aliases.

It is loaded and started by the A35: see `target/stm32mp2_ca35/drivers/copro_control.hh`.

- `boot/startup_cm33.s`: vector table (all external IRQs go to `IRQ_Dispatch`), reset handler
  with boot breadcrumbs, `.data`/`.bss` init, weak `system_init()`, C++ constructors, `main()`.
- `drivers/interrupt_control.hh`, `interrupt_handler.cc`: NVIC, same API as the other targets.
- `drivers/hal_tick_stgen.cc`: `HAL_GetTick()`/`udelay()`/`mdelay()` from the system timestamp
  generator (STGEN), the same counter as the A35's CNTPCT. No SysTick needed.
- `drivers/cache.hh`: no-ops for now (the M33 caches aren't enabled yet).
