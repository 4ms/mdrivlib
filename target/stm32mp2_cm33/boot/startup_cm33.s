/* Cortex-M33 startup for the STM32MP25x coprocessor (CPU2).
 *
 * The A35 loads the image, points CA35SYSCFG->M33_INITSVTOR_CR at this vector table
 * (the M33 runs secure), and releases CPU2 from hold-boot (see
 * target/stm32mp2_ca35/drivers/copro_control.hh). On reset the M33 loads SP from
 * vector[0] and PC from vector[1].
 *
 * Every external interrupt vector points to IRQ_Dispatch (drivers/interrupt_handler.cc),
 * which calls the ISR registered with mdrivlib::InterruptManager.
 *
 * Boot breadcrumbs, for the A35 (or a debugger) to check how far the M33 got:
 *   vector slot 8 (offset 0x20) = 0xB007C0DE: the reset handler ran
 *   vector slot 9 (offset 0x24) = 0xB007C0DF: about to call main()
 *
 * Required from the app/linker: _estack, _sidata, _sdata, _edata, _sbss, _ebss, main().
 * Weak hook: system_init() (called before C++ static constructors).
 */
	.syntax unified
	.cpu cortex-m33
	.fpu fpv5-sp-d16
	.thumb

	.equ NumExternalIRQs, 311 /* IRQn 0..310 (DDRPERFM_IRQn = 310) */

	.section .text.Reset_Handler,"ax",%progbits
	.global Reset_Handler
	.type Reset_Handler, %function
Reset_Handler:
	ldr   sp, =_estack

	ldr   r0, =g_pfnVectors
	ldr   r1, =0xB007C0DE
	str   r1, [r0, #0x20]
	dsb

	/* Enable CP10/CP11 (FPU) full access: SCB->CPACR |= (0xF << 20) */
	ldr   r0, =0xE000ED88
	ldr   r1, [r0]
	orr   r1, r1, #(0xF << 20)
	str   r1, [r0]
	dsb
	isb

	/* Copy .data from its load address (a no-op when LMA == VMA) */
	ldr   r0, =_sdata
	ldr   r1, =_edata
	ldr   r2, =_sidata
	b     2f
1:	ldr   r3, [r2], #4
	str   r3, [r0], #4
2:	cmp   r0, r1
	bcc   1b

	/* Zero .bss */
	ldr   r0, =_sbss
	ldr   r1, =_ebss
	movs  r2, #0
	b     4f
3:	str   r2, [r0], #4
4:	cmp   r0, r1
	bcc   3b

	bl    system_init
	bl    __libc_init_array

	ldr   r0, =g_pfnVectors
	ldr   r1, =0xB007C0DF
	str   r1, [r0, #0x24]
	dsb

	bl    main
5:	b     5b
	.size Reset_Handler, .-Reset_Handler

	.section .text.Default_Handler,"ax",%progbits
	.global Default_Handler
	.type Default_Handler, %function
Default_Handler:
	b     Default_Handler
	.size Default_Handler, .-Default_Handler

	.weak system_init
	.type system_init, %function
system_init:
	bx    lr

	.macro weak_handler name
	.weak \name
	.thumb_set \name, Default_Handler
	.endm

	weak_handler NMI_Handler
	weak_handler HardFault_Handler
	weak_handler MemManage_Handler
	weak_handler BusFault_Handler
	weak_handler UsageFault_Handler
	weak_handler SecureFault_Handler
	weak_handler SVC_Handler
	weak_handler DebugMon_Handler
	weak_handler PendSV_Handler
	weak_handler SysTick_Handler

	.section .isr_vector,"a",%progbits
	.align 11 /* VTOR alignment: 327 vectors -> next power of 2 is 512 words = 2kB */
	.global g_pfnVectors
	.type g_pfnVectors, %object
g_pfnVectors:
	.word _estack
	.word Reset_Handler
	.word NMI_Handler
	.word HardFault_Handler
	.word MemManage_Handler
	.word BusFault_Handler
	.word UsageFault_Handler
	.word SecureFault_Handler
	.word 0 /* breadcrumb 0 */
	.word 0 /* breadcrumb 1 */
	.word 0
	.word SVC_Handler
	.word DebugMon_Handler
	.word 0
	.word PendSV_Handler
	.word SysTick_Handler
	.rept NumExternalIRQs
	.word IRQ_Dispatch
	.endr
	.size g_pfnVectors, .-g_pfnVectors
