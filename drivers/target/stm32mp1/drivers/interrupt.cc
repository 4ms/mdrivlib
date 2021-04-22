#include "interrupt.hh"

static void ISRHandler(unsigned irqnum) {
	InterruptManager::callISR(irqnum);
}

extern "C" {

void __attribute__((interrupt)) SVC_Handler() {
	__BKPT();
}

void __attribute__((interrupt)) IRQ_Handler() {
	IRQn_ID_t irqn = IRQ_GetActiveIRQ();
	if (irqn < 255)
		ISRHandler(irqn);
	IRQ_EndOfInterrupt(irqn);
}

void __attribute__((interrupt)) FIQ_Handler() {
	__BKPT();
}
}
