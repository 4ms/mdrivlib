.syntax unified
.cpu cortex-a7
/* .fpu softvfp */
/* .thumb */

.equ MODE_FIQ, 0x11
.equ MODE_IRQ, 0x12
.equ MODE_SVC, 0x13
.equ MODE_ABT, 0x17
.equ MODE_UND, 0x1B
.equ MODE_SYS, 0x1F

.equ UART4_TDR, 0x40010028

.section .vector_table, "x"
.global _Reset
.global _start
_Reset:
    b Reset_Handler
    b Undef_Handler /* 0x4  Undefined Instruction */
    b SVC_Handler /* Software Interrupt */
    b PAbt_Handler  /* 0xC  Prefetch Abort */
    b DAbt_Handler /* 0x10 Data Abort */
    b . /* 0x14 Reserved */
    b IRQ_Handler /* 0x18 IRQ */
    b FIQ_Handler /* 0x1C FIQ */

.section .text
Reset_Handler:
	CPSID   if 										/* Mask Interrupts */

													/* Put any cores other than 0 to sleep */
	MRC     p15, 0, R0, c0, c0, 5             		/* Read MPIDR */
	ANDS    R0, R0, #3                        
goToSleep:                                
	WFINE                                     
	BNE     goToSleep                         

	
	ldr r4, =UART4_TDR 								/* UART: print 'A' */
	mov r0, #65
	str r0, [r4]


	MRC     p15, 0, R0, c1, c0, 0                   /* Read CP15 System Control register*/
	BIC     R0, R0, #(0x1 << 12)                    /* Clear I bit 12 to disable I Cache*/
	BIC     R0, R0, #(0x1 <<  2)                    /* Clear C bit  2 to disable D Cache*/
	BIC     R0, R0, #0x1                            /* Clear M bit  0 to disable MMU*/
	BIC     R0, R0, #(0x1 << 11)                    /* Clear Z bit 11 to disable branch prediction*/
	BIC     R0, R0, #(0x1 << 13)                    /* Clear V bit 13 to disable hivecs*/
	MCR     p15, 0, R0, c1, c0, 0                   /* Write value back to CP15 System Control register*/
	ISB                                             
													/* Configure ACTLR */
	MRC     p15, 0, r0, c1, c0, 1                   /* Read CP15 Auxiliary Control Register */
	ORR     r0, r0, #(1 <<  1)                      /* Enable L2 prefetch hint (UNK/WI since r4p1) */
	MCR     p15, 0, r0, c1, c0, 1                   /* Write CP15 Auxiliary Control Register */

	// Set Vector Base Address Register (VBAR) to point to this application's vector table
	LDR    R0, =0xC2000040
	MCR    p15, 0, R0, c12, c0, 0

    /* FIQ stack */
    msr cpsr_c, MODE_FIQ
    ldr r1, =_fiq_stack_start
    ldr sp, =_fiq_stack_end
    movw r0, #0xFEFE
    movt r0, #0xFEFE

fiq_loop:
    cmp r1, sp
    strlt r0, [r1], #4
    blt fiq_loop

    /* IRQ stack */
    msr cpsr_c, MODE_IRQ
    ldr r1, =_irq_stack_start
    ldr sp, =_irq_stack_end

irq_loop:
    cmp r1, sp
    strlt r0, [r1], #4
    blt irq_loop

    /* Supervisor mode */
    msr cpsr_c, MODE_SVC
    ldr r1, =_svc_stack_start
    ldr sp, =_svc_stack_end

svc_loop:
    cmp r1, sp
    strlt r0, [r1], #4
    blt svc_loop

	// Initialize nested ISR count
	ldr r0, =0
	ldr r2, =_svc_nested_isr_count
	str r0, [r2]

	/* USER and SYS mode */
	msr cpsr_c, MODE_SYS
	ldr sp, =_user_stack_end

    /* Start copying data */
    ldr r0, =_text_end
    ldr r1, =_data_start
    ldr r2, =_data_end

data_loop:
    cmp r1, r2
    ldrlt r3, [r0], #4
    strlt r3, [r1], #4
    blt data_loop

    /* Initialize .bss */
    mov r0, #0
    ldr r1, =_bss_start
    ldr r2, =_bss_end

bss_loop:
    cmp r1, r2
    strlt r0, [r1], #4
    blt bss_loop

	/* UART: print 'B' */
	mov r5, #66
	str r5, [r4]

	bl SystemInit
    bl __libc_init_array

	/* UART: print 'C' */
	mov r5, #67
	str r5, [r4]

	CPSIE  if 									/* Unmask interrupts */

run_main:
    bl main
    b Abort_Exception

Abort_Exception:
	b .

Undef_Handler:
	b .

PAbt_Handler:
	b .

DAbt_Handler:
	b .

IRQ_Handler:
	sub lr, lr, #4

	//SRSDB sp!, MODE_SYS 	// Save LR_irq and SPSR_irq onto the System mode stack, and update SP_sys
	srsdb sp!, MODE_SYS 		// ??ZZ: Save LR_irq and SPSR_irq onto the System mode stack 

	cps MODE_SYS 		 	// Switch to system mode

	//PUSH {r0-r3, r12} 	// Store remaining AAPCS registers on the System mode stack
	push {r0-r3, r12, lr} 	// ZZ: Store remaining AAPCS registers on the System mode stack

	//
	cps MODE_SVC 			// ZZ: Use SVC mode stack instead of USR/SYS, so ISR has plenty of stack available

	and r3, sp, #4  	 	// Ensure stack is 8-byte aligned. 
	sub sp, sp, r3 

	//push {r1, lr}  		// Store adjustment and LR_sys to SYS stack 
	push {r2, r3}  			// ZZ: Store adjustment and r2(why?) to SVC stack

	ldr r2, =_svc_nested_isr_count
	ldr r0, [r2]
	add r0, r0, #1
	str r0, [r2]

	bl IRQ_GetActiveIRQ

	push {r0, r1} 			//ZZ: Why?	
	//lsl r0, r0, #3

	cpsie i 				// Enable interrupts

	bl ISRHandler
	pop {r0, r1}
	bl IRQ_EndOfInterrupt

	//Pre-empt here

	cpsid i 				// Disable interrupts while we exit

							// ZZ: Decrement nesting count
	ldr r2, =_svc_nested_isr_count
	ldr r0, [r2]
	sub r0, r0, #1
	str r0, [r2]


	//pop {r1, lr} 			// Restore LS_sys 
	// add sp, sp, r1 		// Unadjust stack

	pop {r2, r3} 			// ZZ: Restore previous stack pointer
	add sp, sp, r3  		// ZZ: Restore previous stack pointer

	cps #MODE_SYS
	pop {r0-r3, r12, lr}
	rfeia sp!
	



