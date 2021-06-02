#pragma once
// #include "core_ca7.h"
#include "rcc.hh"
#include "stm32xx.h"

extern uint32_t _Reset; // defined in startup.s

// Controls MPU1 from MPU0
struct SecondaryCoreController {

	enum { MagicNumberValue = 0xCA7FACE1 };
	// static inline volatile uint32_t *MagicNumberRegister = &(TAMP->BKP4R);

	static constexpr auto irq = SGI0_IRQn;

	// static void GIC_SendSecureSGI(IRQn_Type IRQn, uint32_t target_list, uint32_t filter_list) {
	// 	GICDistributor->SGIR =
	// 		((filter_list & 3U) << 24U) | ((target_list & 0xFFUL) << 16U) | (1 << 15) | (IRQn & 0x0FUL);
	// }

	// From https://wiki.st.com/stm32mpu/wiki/STM32MP15_ROM_code_overview#Secondary_core_boot
	// To unpark the core1, the application running on core0 shall:
	// 	- write jump address into BRANCH_ADDRESS backup register.
	// 	- write 0xCA7FACE1 value into MAGIC_NUMBER backup register.
	// 	- generate an SGI interrupt to core1

	static void start() {

		// Enable SMP
		// uint32_t actlr = __get_ACTLR();
		// actlr |= ACTLR_SMP_Msk;
		// __set_ACTLR(actlr);
		// __ISB();
		// __DSB();

		setup_sgi();
		unlock_backup_registers();

		// ROM code needs an SGI0 after core reset
		// core is ready when magic is set to 0 in ROM code
		// reset_magic_number();
		reset();

		write_magic_number(MagicNumberValue);

		while (TAMP->BKP4R)
			send_sgi();

		// Write entry point
		write_branch_address(reinterpret_cast<uint32_t>(&_Reset));

		write_magic_number(MagicNumberValue);

		// Send second SGI0 to start the core
		send_sgi();
	}

	static void unlock_backup_registers() {
		// Turn on the Disable Backup Protection bit, to allow us to write to the TAMP backup registers
		PWR->CR1 = PWR->CR1 | PWR_CR1_DBP;
		while (!(PWR->CR1 & PWR_CR1_DBP))
			;
	}

	static void reset_magic_number() {
		// if (TAMP->BKP4R)
		TAMP->BKP4R = 0xFFFFFFFF;
	}

	static void write_magic_number(uint32_t magic_number_value) {
		TAMP->BKP4R = magic_number_value;
	}

	static void write_branch_address(uint32_t addr) {
		TAMP->BKP5R = addr;
	}

	static void setup_sgi() {
		GIC_EnableIRQ(irq);
		GIC_SetPriority(irq, 0b00000000);
	}
	static void send_sgi() {
		constexpr uint32_t filter_to_this_core_only = 0b01;
		constexpr uint32_t filter_to_all_other_cores = 0b01;
		constexpr uint32_t filter_use_cpu_sel_bits = 0b00;
		constexpr uint32_t cpu0 = 1 << 0;
		constexpr uint32_t cpu1 = 1 << 1;

		uint32_t periphbase;

		/* get the GIC base address from the CBAR register */

		constexpr uint32_t CBAR_MASK = 0xFFFF8000;
		constexpr uint32_t GIC_DIST_OFFSET = 0x1000;
		constexpr uint32_t GICD_SGIR = 0x0f00;
		asm("mrc p15, 4, %0, c15, c0, 0\n" : "=r"(periphbase));
		volatile uint32_t *SGIR = reinterpret_cast<uint32_t *>((periphbase & CBAR_MASK) + GIC_DIST_OFFSET + GICD_SGIR);
		asm volatile("msr cpsr_c, #22\n");
		*SGIR = 1UL << 17UL;
		__DSB();
		__ISB();
		asm volatile("msr cpsr_c, #0x1f\n");
		// GIC_SendSGI(irq, cpu1, filter_use_cpu_sel_bits);
	}

	static void reset() {
		// Reset MPU1
		RCC->MP_GRSTCSETR = RCC_MP_GRSTCSETR_MPUP1RST;
		__DSB();
		__ISB();
	}

	static void stop() {
		// Stop request for core 1
		// Todo: Does this work?
		RCC->MP_SREQSETR = RCC_MP_SREQSETR_STPREQ_P1;
		__DSB();
		__ISB();
	}
};

using SecondaryCore = SecondaryCoreController;
