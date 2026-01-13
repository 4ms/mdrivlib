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

// void SystemInit(void) {
// 	// do not use global variables because this function is called before
// 	// reaching pre-main. RW section may be overwritten afterwards.

// 	// Force reset of all peripherals
// 	// This facilitates resetting the system while debugging
// 	RCC->APB1RSTSETR = 0xFFFFFFFF;
// 	RCC->APB1RSTCLRR = 0xFFFFFFFF;

// 	RCC->APB2RSTSETR = 0xFFFFFFFF;
// 	RCC->APB2RSTCLRR = 0xFFFFFFFF;

// 	RCC->APB3RSTSETR = 0xFFFFFFFF;
// 	RCC->APB3RSTCLRR = 0xFFFFFFFF;

// 	RCC->APB4RSTSETR = 0xFFFFFFFF;
// 	RCC->APB4RSTCLRR = 0xFFFFFFFF;

// 	RCC->APB5RSTSETR = 0xFFFFFFFF;
// 	RCC->APB5RSTCLRR = 0xFFFFFFFF;

// 	RCC->APB6RSTSETR = 0xFFFFFFFF;
// 	RCC->APB6RSTCLRR = 0xFFFFFFFF;

// 	RCC->AHB2RSTSETR = 0xFFFFFFFF;
// 	RCC->AHB2RSTCLRR = 0xFFFFFFFF;

// 	RCC->AHB4RSTSETR = 0xFFFFFFFF;
// 	RCC->AHB4RSTCLRR = 0xFFFFFFFF;

// 	// Invalidate entire Unified TLB
// 	__set_TLBIALL(0);

// 	// Invalidate entire branch predictor array
// 	__set_BPIALL(0);
// 	__DSB();
// 	__ISB();

// 	//  Invalidate instruction cache and flush branch target cache
// 	__set_ICIALLU(0);
// 	__DSB();
// 	__ISB();

// 	L1C_InvalidateDCacheAll();

// 	__FPU_Enable();

// 	MMU_CreateTranslationTable();
// 	MMU_Enable();

// 	L1C_EnableCaches();
// 	L1C_EnableBTAC();

// #if (__L2C_PRESENT == 1)
// 	L2C_Enable();
// #endif

// 	IRQ_Initialize();
// }
