#include "drivers/interrupt.hh"

static void ISRHandler(unsigned irqnum) {
	InterruptManager::callISR(irqnum);
}

extern "C" {

void __attribute__((interrupt)) SVC_Handler() {
	__BKPT();
}

void __attribute__((interrupt)) IRQ_Handler() {
	asm volatile(""
				 "SUB lr, lr, #4 	\n"
				 "SRSFD sp!, #0x1f 	\n" // use SRS to save LR_irq and SPSR_irq in one step onto the System mode stack
				 "CPS #0x1f 		\n" // Use CPS to switch to system mode"
				 "PUSH {r0-r3, r12} \n" // Store remaining AAPCS registers on the System mode stack
				 "AND r1, sp, #4  	\n" // Ensure stack is 8-byte aligned. Store adjustment and
										// LR_sys to stack
				 "SUB sp, sp, r1 	\n"
				 "PUSH {r1, lr} 	\n");
	IRQn_ID_t irqn = IRQ_GetActiveIRQ();
	__enable_irq();
	if (irqn < 255) {
		ISRHandler(irqn);
	}
	IRQ_EndOfInterrupt(irqn);
	asm volatile("CPSID i 			\n"	  // Disable IRQ with CPS
				 "POP {r1, lr} 		\n"	  // Restore LS_sys
				 "ADD sp, sp, r1  	\n"	  // Unadjust stack
				 "POP {r0-r3, r12}  \n"	  // Restore AAPCS registers
				 "RFEFD sp! 		\n"); // REturn using RFE from the System mode stack
}

void C_irq_handler() {
}

void __attribute__((interrupt)) FIQ_Handler() {
	__BKPT();
}
}
