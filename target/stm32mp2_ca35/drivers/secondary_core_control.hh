#pragma once
#include "drivers/smc.hh"
#include "drivers/stm32xx.h"
#include "drivers/system_reg.hh"
#include <cstdint>

namespace mdrivlib
{

struct SecondaryCore {
	static constexpr uint64_t PSCI_CPU_ON_AARCH64 = 0xC4000003ULL;

	// Start the second A35 core at the given entry point
	// (typically aux_core_startup from boot/aux_core_startup.s, which calls aux_main()).
	// At EL3 this resets the core directly; at lower ELs it uses PSCI CPU_ON via the
	// Secure Monitor (context is only passed in the PSCI case).
	// Returns 0 on success.
	static int start(void (*cpu1_entry)(), uint64_t context = 0) {
		uint64_t mpidr0 = get_mpid();

		// Keep affinity levels except AFF0, then set AFF0=1
		// (AFF0 is bits[7:0] in MPIDR on Armv8-A)
		uint64_t target = (mpidr0 & ~0xFFULL) | 1ULL;

		if (get_current_el() == 3) {
			// Set reset vector for CPU1 in 64-bit mode
			CA35SYSCFG->VBAR_CR = ((uint32_t)(uintptr_t)cpu1_entry) & ~0b11;

			// Reset CPU1 processor core 1
			RCC->C1P1RSTCSETR = RCC_C1P1RSTCSETR_C1P1RST;
			while (RCC->C1P1RSTCSETR & RCC_C1P1RSTCSETR_C1P1RST) {
				;
			}

			return 0;
		} else {
			auto ret = smc_call(PSCI_CPU_ON_AARCH64, target, (uint64_t)cpu1_entry, context, 0, 0, 0, 0);

			return ret.a0; // 0 means success
		}
	}

	// Send a Software Generated Interrupt to another core.
	// target_list: bit per CPU interface (0b01 = core 0, 0b10 = core 1)
	static void send_sgi(IRQn_Type sgi_irqn, uint32_t target_list) {
		GIC_SendSGI(sgi_irqn, target_list, 0b00);
	}
};

} // namespace mdrivlib
