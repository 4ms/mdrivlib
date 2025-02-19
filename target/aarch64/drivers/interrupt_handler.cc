#include "drivers/interrupt.hh"
#define DEBUG_IRQ
#if defined(DEBUG_IRQ)
#include <cstdio>
#endif

extern "C" void __attribute__((used)) ISRHandler(unsigned irqnum) {
#if defined(DEBUG_IRQ)
	printf("IRQ %u\n", irqnum);
#endif
	mdrivlib::InterruptManager::callISR(irqnum);
}
