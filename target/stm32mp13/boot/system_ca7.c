#include "drivers/stm32xx.h"
#include "irq_ctrl.h"
#include "mmu_ca7.h"

uint32_t SystemCoreClock = HSE_VALUE;

void SystemInit(void) {

	MMU_InvalidateTLB();

	/* Disable all interrupts and events */
	CLEAR_REG(EXTI_C1->IMR1);
	CLEAR_REG(EXTI_C1->IMR2);
	CLEAR_REG(EXTI_C1->IMR3);
	CLEAR_REG(EXTI_C1->EMR1);
	CLEAR_REG(EXTI_C1->EMR2);
	CLEAR_REG(EXTI_C1->EMR3);

	L1C_InvalidateBTAC();

	L1C_InvalidateICacheAll();

	L1C_InvalidateDCacheAll();

	__FPU_Enable();

	MMU_CreateTranslationTable();

	MMU_Enable();

	L1C_EnableCaches();
	L1C_EnableBTAC();

	IRQ_Initialize();

	// Start system timer, halt on debug
	STGENC->CNTSR = 0b11;
}
