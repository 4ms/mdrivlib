#include "drivers/interrupt.hh"

extern "C" void __attribute__((used)) ISRHandler(unsigned irqnum) {
	mdrivlib::InterruptManager::callISR(irqnum);
}
