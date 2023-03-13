#pragma once
#include "drivers/rcc.hh"
#include "drivers/stm32xx.h"

namespace mdrivlib
{

struct System {
	System() = delete;

	static constexpr uint32_t F030_VTableSize = 28;

	static void SetVectorTable(uint32_t reset_address) {
		if (reset_address == 0x08000000)
			return;

		__HAL_RCC_SYSCFG_CLK_ENABLE();

		// Copy the vector table from the Flash (mapped at the base of the application
		// load address) to the base address of the SRAM at 0x20000000.
		// NOTE: You must setup linker script to reserve this space like this:
		// MEMORY {
		// 		VTRAM (xrw)     : ORIGIN = 0x20000000, LENGTH = 0x70  /* First part of RAM is reserved to the vector table */
		// 		RAM (xrw)       : ORIGIN = 0x20000070, LENGTH = 4K - 0x70
		// 		...
		// }
		uint32_t SRAM_addr = 0x20000000;
		auto *dst = reinterpret_cast<uint32_t *>(SRAM_addr);
		auto *src = reinterpret_cast<uint32_t *>(reset_address);
		for (int i = 0; i < F030_VTableSize; i++) {
			*dst++ = *src++;
		}

		// Remap SRAM 0x20000000 to 0x00000000
		// Core uses 0x00000000 + IRQNum*4 for interrupts
		SYSCFG->CFGR1 = SYSCFG->CFGR1 | SYSCFG_CFGR1_MEM_MODE_0 | SYSCFG_CFGR1_MEM_MODE_1;
	}

	static void poweron() {
		__HAL_RCC_SYSCFG_CLK_ENABLE();
		__HAL_RCC_PWR_CLK_ENABLE();
		// __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	}

	static void reset_buses() {
		RCC_Reset::AHB::set();
		RCC_Reset::AHB::clear();

		RCC_Reset::APB1::set();
		RCC_Reset::APB1::clear();

		RCC_Reset::APB2::set();
		RCC_Reset::APB2::clear();
	}

	static void reset_RCC() {
		HAL_RCC_DeInit();

		// /* Set HSION bit, HSITRIM[4:0] bits to the reset value*/
		// RCC->CR = RCC->CR | RCC_CR_HSION | RCC_CR_HSITRIM_4;

		// /* Wait till HSI is ready */
		// while ((RCC->CR & RCC_CR_HSIRDY) == RESET) {
		// }

		// /* Reset SW[1:0], HPRE[3:0], PPRE[2:0] and MCOSEL[2:0] bits */
		// RCC->CFGR = RCC->CFGR & ~(RCC_CFGR_SW | RCC_CFGR_HPRE | RCC_CFGR_PPRE | RCC_CFGR_MCO);

		// /* Wait till HSI as SYSCLK status is enabled */
		// while ((RCC->CFGR & RCC_CFGR_SWS) != RESET) {
		// }

		// /* Update the SystemCoreClock global variable for HSI as system clock source */
		// SystemCoreClock = HSI_VALUE;

		// /* Adapt Systick interrupt period */
		// HAL_InitTick(uwTickPrio);

		// /* Reset HSEON, CSSON, PLLON bits */
		// RCC->CR = RCC->CR & ~(RCC_CR_PLLON | RCC_CR_CSSON | RCC_CR_HSEON);

		// /* Reset HSEBYP bit */
		// RCC->CR = RCC->CR & ~(RCC_CR_HSEBYP);

		// /* Wait till PLLRDY is cleared */
		// while ((RCC->CR & RCC_CR_PLLRDY) != RESET) {
		// }

		// /* Reset CFGR register */
		// CLEAR_REG(RCC->CFGR);

		// /* Reset CFGR2 register */
		// CLEAR_REG(RCC->CFGR2);

		// /* Reset CFGR3 register */
		// CLEAR_REG(RCC->CFGR3);

		// /* Disable all interrupts */
		// CLEAR_REG(RCC->CIR);

		// /* Clear all reset flags */
		// RCC->CSR = RCC->CSR | RCC_CSR_RMVF;
	}
};

} // namespace mdrivlib
