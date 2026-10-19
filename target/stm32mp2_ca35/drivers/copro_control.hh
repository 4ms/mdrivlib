#pragma once
#include "drivers/cache.hh"
#include "drivers/system_reg.hh"
#include "drivers/stm32xx.h"
#include <cstdint>
#include <span>

// Load and start the Cortex-M33 coprocessor (CPU2) from the A35.
// Ported from stm32mp2-baremetal shared/drivers/copro_m33.hh (verified on STM32MP257 hardware).
//
// The M33 runs secure:
// - CA35SYSCFG->M33_TZEN_CR CFG_SECEXT = 1, and the image's vector table goes in the
//   secure vector register M33_INITSVTOR_CR. The image must be linked at that address.
// - A secure instruction fetch to a non-secure RISAB page is an illegal access (RISAB SRWIAD
//   only forgives data accesses), and block_ram_enable_el3() made all SRAM pages non-secure
//   at boot, so the SRAM2 pages are made secure again before starting the M33.

namespace mdrivlib
{
struct CoproController {
	// SRAM2, as seen by the M33's secure instruction fetch (and mapped there for the A35 too)
	static constexpr uintptr_t SRAM2_Start = 0x0E060000;
	static constexpr uintptr_t SRAM2_End = 0x0E080000;

	// Hold CPU2 in reset with hold-boot asserted (mirrors OP-TEE rproc_stop()).
	// Do this before re-initializing anything the M33 uses (HSEM, IPCC, shared memory).
	static void reset() {
		RCC->CPUBOOTCR &= ~RCC_CPUBOOTCR_BOOT_CPU2;
		RCC->C2RSTCSETR = RCC_C2RSTCSETR_C2RST;
	}

	// Copy the M33 image to the address it was linked at, and clean it from the A35's cache
	static void load_firmware(std::span<const uint8_t> image, uintptr_t load_addr) {
		auto *dst = reinterpret_cast<volatile uint8_t *>(load_addr);
		for (unsigned i = 0; i < image.size(); i++)
			dst[i] = image[i];

		SystemCache::clean_dcache_by_range(reinterpret_cast<void *>(load_addr), image.size());
		dsb_sy();
		isb();
	}

	// Start the M33 with its vector table at vector_table_addr (secure alias).
	// Releasing hold-boot also releases the M33 reset (see OP-TEE stm32_rproc_start()).
	static void start(uintptr_t vector_table_addr) {
		if (vector_table_addr >= SRAM2_Start && vector_table_addr < SRAM2_End)
			sram2_set_secure();

		CA35SYSCFG->M33_TZEN_CR |= CA35SYSCFG_M33_TZEN_CR_CFG_SECEXT;
		CA35SYSCFG->M33_INITSVTOR_CR = vector_table_addr & CA35SYSCFG_M33_INITSVTOR_CR_INITSVTOR_Msk;
		dsb_sy();
		isb();

		RCC->CPUBOOTCR |= RCC_CPUBOOTCR_BOOT_CPU2;
	}

	// reset(), load_firmware(), start()
	static void load_and_start(std::span<const uint8_t> image, uintptr_t load_addr) {
		reset();
		load_firmware(image, load_addr);
		start(load_addr);
	}

	// Boot breadcrumbs written by target/stm32mp2_cm33/boot/startup_cm33.s
	static constexpr uint32_t BreadcrumbResetRan = 0xB007C0DE;
	static constexpr uint32_t BreadcrumbReachedMain = 0xB007C0DF;
	static uint32_t read_breadcrumb(uintptr_t vector_table_addr, unsigned num) {
		auto addr = vector_table_addr + 0x20 + num * 4;
		SystemCache::invalidate_dcache_by_range(reinterpret_cast<void *>(addr), 4);
		return *reinterpret_cast<volatile uint32_t *>(addr);
	}

	static int32_t wait_for_breadcrumb(uintptr_t vector_table_addr, unsigned num, uint32_t value, uint32_t timeout_ms) {
		auto freq = read_cntfreq();
		auto start = read_cntpct();
		auto timeout_ticks = freq / 1000 * timeout_ms;
		while (true) {
			auto elapsed = read_cntpct() - start;
			if (read_breadcrumb(vector_table_addr, num) == value)
				return int32_t(elapsed * 1'000'000 / freq);
			if (elapsed > timeout_ticks)
				return -1;
		}
	}

private:
	static void sram2_set_secure() {
		for (auto i = 0u; i < 32; i++)
			RISAB4->PGSECCFGR[i] = 0xFF; // all 8 blocks of each page secure
	}
};

using Copro = CoproController;
} // namespace mdrivlib
