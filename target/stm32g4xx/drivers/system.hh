#pragma once
#include "drivers/stm32xx.h"

namespace mdrivlib
{

struct System {
	System() = delete;

	static void SetVectorTable(uint32_t reset_address) {
		SCB->VTOR = reset_address & (uint32_t)0x1FFFFF80;
	}

	using EntryPoint = void (*)();

	static void jump_to(uint32_t address) {
		uint32_t application_address = *(volatile uint32_t *)(address + 4);
		auto application = (EntryPoint)(application_address);
		__set_MSP(*(volatile uint32_t *)address);
		application();
	}

	static void reset_buses() {
		__HAL_RCC_APB1_FORCE_RESET();
		__HAL_RCC_APB1_RELEASE_RESET();

		__HAL_RCC_APB2_FORCE_RESET();
		__HAL_RCC_APB2_RELEASE_RESET();

		__HAL_RCC_AHB1_FORCE_RESET();
		__HAL_RCC_AHB1_RELEASE_RESET();

		__HAL_RCC_AHB2_FORCE_RESET();
		__HAL_RCC_AHB2_RELEASE_RESET();

		__HAL_RCC_AHB3_FORCE_RESET();
		__HAL_RCC_AHB3_RELEASE_RESET();
	}

	static void reset_RCC() {
		// SysTick->CTRL &= ~(SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk |
		// 				   SysTick_CTRL_ENABLE_Msk); /* Disable SysTick IRQ and SysTick Timer */

		// /* Set HSION bit to the reset value */
		// SET_BIT(RCC->CR, RCC_CR_HSION);

		// /* Wait till HSI is ready */
		// while (READ_BIT(RCC->CR, RCC_CR_HSIRDY) == RESET) {
		// 	;
		// }

		// /* Set HSITRIM[4:0] bits to the reset value */
		// SET_BIT(RCC->CR, RCC_CR_HSITRIM_4);

		// /* Reset CFGR register */
		// CLEAR_REG(RCC->CFGR);

		// /* Wait till clock switch is ready */
		// while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET) {
		// 	;
		// }

		// /* Clear HSEON, HSEBYP and CSSON bits */
		// CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_HSEBYP | RCC_CR_CSSON);

		// /* Wait till HSE is disabled */
		// while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET) {
		// 	;
		// }

		// /* Clear PLLON bit */
		// CLEAR_BIT(RCC->CR, RCC_CR_PLLON);

		// /* Wait till PLL is disabled */
		// while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET) {
		// 	;
		// }

		// /* Reset PLLI2SON bit */
		// CLEAR_BIT(RCC->CR, RCC_CR_PLLI2SON);

		// /* Wait till PLLI2S is disabled */
		// while (READ_BIT(RCC->CR, RCC_CR_PLLI2SRDY) != RESET) {
		// 	;
		// }

		// /* Disable all interrupts */
		// CLEAR_BIT(RCC->CIR,
		// 		  RCC_CIR_LSIRDYIE | RCC_CIR_LSERDYIE | RCC_CIR_HSIRDYIE | RCC_CIR_HSERDYIE | RCC_CIR_PLLRDYIE |
		// 			  RCC_CIR_PLLI2SRDYIE);

		// /* Clear all interrupt flags */
		// SET_BIT(RCC->CIR,
		// 		RCC_CIR_LSIRDYC | RCC_CIR_LSERDYC | RCC_CIR_HSIRDYC | RCC_CIR_HSERDYC | RCC_CIR_PLLRDYC |
		// 			RCC_CIR_PLLI2SRDYC | RCC_CIR_CSSC);

		// /* Clear LSION bit */
		// CLEAR_BIT(RCC->CSR, RCC_CSR_LSION);

		// /* Reset all CSR flags */
		// SET_BIT(RCC->CSR, RCC_CSR_RMVF);

		// /* Update the SystemCoreClock global variable */
		// SystemCoreClock = HSI_VALUE;
	}
};

} // namespace mdrivlib
