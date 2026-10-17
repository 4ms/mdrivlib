#include "drivers/interrupt.hh"

// Every external-interrupt vector in boot/startup_cm33.s points here. The active
// exception number (16 + IRQn) is read back from the SCB, and the ISR registered
// with InterruptManager for that IRQn is called.
extern "C" void __attribute__((used)) IRQ_Dispatch() {
	const unsigned exception_num = SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk;
	mdrivlib::InterruptManager::callISR(exception_num - 16);
}
