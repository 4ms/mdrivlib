#include "drivers/interrupt.hh"

extern "C" /*__attribute__((interrupt))*/ void ISRHandler(unsigned irqnum) {
	InterruptManager::callISR(irqnum);
}

extern "C" {
void __attribute__((interrupt)) SVC_Handler() {
	__BKPT();
}

void __attribute__((interrupt)) IRQ_Handler() {
	asm volatile(
		".equ MODE_SYS, 0x1F  			\n"
		".equ GICCPU_BASE_low, 0x2000 	\n"
		".equ GICCPU_BASE_high, 0xA002 	\n"
		".equ GICC_HPPIR_offset, 24   	\n"
		".equ GICC_IAR_offset, 12  		\n"
		"sub lr, lr, #4 				\n" // LR = addr of *next* instr to execute; subtract 4 so we return to instr
											// that was about to be executed
		"srsdb sp!, MODE_SYS 			\n" // Save LR_irq and SPSR_irq onto the System mode stack, (decrement SP_sys)
		"cps MODE_SYS 		 			\n" // Switch to system mode
		"push {r0-r3, r12, lr} 			\n" // Store remaining AAPCS registers on the System mode stack
		"and r3, sp, #4  	 			\n" // Ensure stack is 8-byte aligned.
		"sub sp, sp, r3  				\n" //
		"push {r3}  					\n" // Store adjustment to stack

		"mov r3, #GICCPU_BASE_low		\n" // (this block used to be 'bl IRQ_GetActiveIRQ'
		"movt r3, #GICCPU_BASE_high		\n" // Load address of the GIC CPU Interface
		"ldr r2, [r3, #GIC_HPPIR_offset]\n" // Get Highest Pending Interrupt number
		"ldr r2, [r3, #GIC_IAR_offset]	\n" // Acknowledge it with a read to the Interrupt Acknowledge Register
		"uxth r0, r2 					\n" // Unpack it... why?
		"dsb sy							\n" // Data barrier (why?)

		"push {r0, r1} 					\n" // Push IRQ number (r0) and ??? (r1) so ISRHandler doesn't overwrite them
											// (we need them for IRQ_EndOfInterrupt )
		"cpsie i 						\n" // Enable interrupts
		"bl ISRHandler 					\n" // Handle the ISR
		"pop {r0, r1} 					\n" // Restore the IRQ number
		"bl IRQ_EndOfInterrupt  		\n" // Signal the GIC that we're done processing
		"cpsid i 						\n" // Disable interrupts while we exit
		"pop {r3} 						\n" // Pop the stack adjustment
		"add sp, sp, r3  				\n" // Restore previous stack pointer
		"pop {r0-r3, r12, lr} 			\n" //
		"rfeia sp! 						\n" // Return to address on stack, and pop SPSR (which restores the en/disable
											// state of IRQs)
	);
}

void __attribute__((interrupt)) FIQ_Handler() {
	__BKPT();
}
}
