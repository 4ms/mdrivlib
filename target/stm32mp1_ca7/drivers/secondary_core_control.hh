#pragma once
// #include "core_ca7.h"
#include "rcc.hh"
#include "stm32xx.h"

extern uint32_t *_Reset; // defined in startup.s

// Controls MPU1 from MPU0
struct SecondaryCoreController {

	enum {
		MagicNumberValue = 0xCA7FACE1,
	};

	// From https://wiki.st.com/stm32mpu/wiki/STM32MP15_ROM_code_overview#Secondary_core_boot
	// To unpark the core1, the application running on core0 shall:
	// write jump address into BRANCH_ADDRESS backup register.
	// write 0xCA7FACE1 value into MAGIC_NUMBER backup register.
	// generate an SGI interrupt to core1
	static void start() {
		write_branch_address(reinterpret_cast<uint32_t>(_Reset));
		write_magic_number(MagicNumberValue);
		// Send SGI #0 interrupt to core1
		GIC_SendSGI((IRQn_Type)0, 0b10, 0b00);
		__DSB();
		__ISB();
	}

	static void write_magic_number(uint32_t magic_number_value) {
		TAMP->BKP4R = magic_number_value;
	}

	static void write_branch_address(uint32_t addr) {
		TAMP->BKP5R = addr;
	}

	static void reset() {
		// Reset MPU1
		// Todo: Does this work?
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
