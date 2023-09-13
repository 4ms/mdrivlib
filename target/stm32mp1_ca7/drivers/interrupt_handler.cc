#include "drivers/interrupt.hh"

extern "C" {
void __attribute__((used)) ISRHandler(unsigned irqnum) {
	mdrivlib::InterruptManager::callISR(irqnum);
}

void __attribute__((used)) SVC_Handler() {
	__NOP();
	//__BKPT();
}

void __attribute__((naked)) __attribute__((section(".irqhandler"))) IRQ_Handler() {
	asm volatile(
		".equ MODE_SVC, 0x13  			\n"
		".equ MODE_SYS, 0x1F  			\n"
		".equ GICCPU_BASE_low, 0x2000 	\n"
		".equ GICCPU_BASE_high, 0xA002 	\n"
		"sub lr, lr, #4 				\n" // LR = addr of *next* instr to execute; subtract 4 so we return to instr
											// that was about to be executed
		"srsdb sp!, MODE_SYS 			\n" // Save LR_irq and SPSR_irq onto the System mode stack, (decrement SP_sys)
		"cps MODE_SYS 		 			\n" // Switch to system mode (because our baremetal app runs in system mode)
		"push {r0-r3, r12, lr} 			\n" // Store remaining AAPCS registers on the System mode stack:
											// these are expected to be clobbered by the bl ISRHandler call

		"mov r3, #GICCPU_BASE_low		\n"
		"movt r3, #GICCPU_BASE_high		\n" // Load address of the GIC CPU Interface
		"ldr r0, [r3, #12]				\n" // AIAR: Acknowledge it with a read to the Interrupt Acknowledge Register
		// "dsb sy							\n" // Data barrier (Todo: research why)

		"bfc r0, #10, #3		   		\n" // Clear the sending CPU ID bits (which are set for SGIs)
		"cmp r0, #0x03fc 				\n" // Check if it's a valid IRQ number
		"bge ExitISRHandler				\n" // Skip this if it's invalid

		"and r2, sp, #4  	 			\n" // Ensure stack is 8-byte aligned.
		"sub sp, sp, r2  				\n" // r2 contains the stack adjustment

		"vmrs r1, FPSCR 				\n" // Copy FPU status reg to r1
		"vpush {d0-d15} 				\n" // Push all double and single registers
		"vpush {d16-d31} 				\n"

		"push {r0-r3}					\n" // Push IRQ number (r0), FPU Status (r1), stack adj. (r2), and GIC base (r3)
		"cpsie i 						\n" // Enable interrupts
		"bl ISRHandler 					\n" // Handle the ISR
		"cpsid i 						\n" // Disable interrupts while we exit
		"pop {r0-r3} 					\n" // Restore the IRQ number, FPU status, stack adj. and GIC base

		"str r0, [r3, #16] 				\n" // EOIR: Write IRQ num to End of Interrupt Register to tell GIC we're done

		"vpop {d16-d31} 				\n"
		"vpop {d0-d15} 					\n" // Push all double and single registers
		"vmsr FPSCR, r1 				\n" // Restore FPU status reg from popped r1

		"add sp, sp, r2  				\n" // Restore previous stack pointer

		"ExitISRHandler: 				\n"
		"pop {r0-r3, r12, lr} 			\n" //
		"rfeia sp! 						\n" // Return to address on stack, and pop SPSR (which restores the en/disable
											// state of IRQs)
	);
}

void __attribute__((used)) FIQ_Handler() {
	__NOP();
	// __BKPT();
}
}
