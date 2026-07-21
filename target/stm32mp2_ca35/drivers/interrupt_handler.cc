#include "drivers/interrupt.hh"

// Called from the IRQ path in boot/vectors.S, which acknowledges the GIC,
// saves/restores register state, and writes EOIR after this returns.
extern "C" void __attribute__((used)) ISRHandler(unsigned irqnum) {
	mdrivlib::InterruptManager::callISR(irqnum);
}
