#pragma once
// Ported from 4ms/stm32mp2-baremetal (verified on STM32MP257 hardware)
// RISAB/RIF memory firewall: grant full access to the on-chip RAM blocks.

namespace mdrivlib
{
void sysram_enable();
void sram1_enable();
void sram3_enable();
void retram_enable();
void vderam_enable();
} // namespace mdrivlib

// Called (as a weak hook) from the EL3 path in boot/startup_ca35.s
extern "C" void block_ram_enable_el3();
