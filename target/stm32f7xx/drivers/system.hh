#pragma once
#include "drivers/stm32xx.h"

namespace mdrivlib
{

struct System {
	System() = delete;

	static void SetVectorTable(uint32_t reset_address) {
		SCB->VTOR = reset_address & (uint32_t)0x1FFFFF80;
	}

	typedef void (*EntryPoint)(void);
	static void jump_to(uint32_t address) {
		uint32_t application_address = *(volatile uint32_t *)(address + 4);
		EntryPoint application = (EntryPoint)(application_address);
		__set_MSP(*(volatile uint32_t *)address);
		application();
	}

	// TODO: check this is right! From F730
	static void reset_buses() {
		RCC->APB1RSTR = 0xFFFFFFFFU;
		RCC->APB1RSTR = 0x00U;

		RCC->APB2RSTR = 0xFFFFFFFFU;
		RCC->APB2RSTR = 0x00U;

		RCC->AHB1RSTR = 0xFFFFFFFFU;
		RCC->AHB1RSTR = 0x00U;

		RCC->AHB2RSTR = 0xFFFFFFFFU;
		RCC->AHB2RSTR = 0x00U;

		RCC->AHB3RSTR = 0xFFFFFFFFU;
		RCC->AHB3RSTR = 0x00U;
	}

	// TODO: check this is right! From F730
	static void reset_RCC() {
		SysTick->CTRL &= ~(SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk |
						   SysTick_CTRL_ENABLE_Msk); /* Disable SysTick IRQ and SysTick Timer */

		/* Set HSION bit to the reset value */
		SET_BIT(RCC->CR, RCC_CR_HSION);

		/* Wait till HSI is ready */
		while (READ_BIT(RCC->CR, RCC_CR_HSIRDY) == RESET) {
			;
		}

		/* Set HSITRIM[4:0] bits to the reset value */
		SET_BIT(RCC->CR, RCC_CR_HSITRIM_4);

		/* Reset CFGR register */
		CLEAR_REG(RCC->CFGR);

		/* Wait till clock switch is ready */
		while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET) {
			;
		}

		/* Clear HSEON, HSEBYP and CSSON bits */
		CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_HSEBYP | RCC_CR_CSSON);

		/* Wait till HSE is disabled */
		while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET) {
			;
		}

		/* Clear PLLON bit */
		CLEAR_BIT(RCC->CR, RCC_CR_PLLON);

		/* Wait till PLL is disabled */
		while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET) {
			;
		}

		/* Reset PLLI2SON bit */
		CLEAR_BIT(RCC->CR, RCC_CR_PLLI2SON);

		/* Wait till PLLI2S is disabled */
		while (READ_BIT(RCC->CR, RCC_CR_PLLI2SRDY) != RESET) {
			;
		}

		/* Reset PLLSAI bit */
		CLEAR_BIT(RCC->CR, RCC_CR_PLLSAION);

		/* Wait till PLLSAI is disabled */
		while (READ_BIT(RCC->CR, RCC_CR_PLLSAIRDY) != RESET) {
			;
		}

		/* Once PLL, PLLI2S and PLLSAI are OFF, reset PLLCFGR register to default value */
		RCC->PLLCFGR = RCC_PLLCFGR_PLLM_4 | RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLN_7 | RCC_PLLCFGR_PLLQ_2 | 0x20000000U;

		/* Reset PLLI2SCFGR register to default value */
		RCC->PLLI2SCFGR =
			RCC_PLLI2SCFGR_PLLI2SN_6 | RCC_PLLI2SCFGR_PLLI2SN_7 | RCC_PLLI2SCFGR_PLLI2SQ_2 | RCC_PLLI2SCFGR_PLLI2SR_1;

		/* Reset PLLSAICFGR register to default value */
		RCC->PLLSAICFGR = RCC_PLLSAICFGR_PLLSAIN_6 | RCC_PLLSAICFGR_PLLSAIN_7 | RCC_PLLSAICFGR_PLLSAIQ_2 | 0x20000000U;

		/* Disable all interrupts */
		CLEAR_BIT(RCC->CIR,
				  RCC_CIR_LSIRDYIE | RCC_CIR_LSERDYIE | RCC_CIR_HSIRDYIE | RCC_CIR_HSERDYIE | RCC_CIR_PLLRDYIE |
					  RCC_CIR_PLLI2SRDYIE | RCC_CIR_PLLSAIRDYIE);

		/* Clear all interrupt flags */
		SET_BIT(RCC->CIR,
				RCC_CIR_LSIRDYC | RCC_CIR_LSERDYC | RCC_CIR_HSIRDYC | RCC_CIR_HSERDYC | RCC_CIR_PLLRDYC |
					RCC_CIR_PLLI2SRDYC | RCC_CIR_PLLSAIRDYC | RCC_CIR_CSSC);

		/* Clear LSION bit */
		CLEAR_BIT(RCC->CSR, RCC_CSR_LSION);

		/* Reset all CSR flags */
		SET_BIT(RCC->CSR, RCC_CSR_RMVF);

		/* Update the SystemCoreClock global variable */
		SystemCoreClock = HSI_VALUE;
	}
};

} // namespace mdrivlib
