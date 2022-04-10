#include "interrupt.hh"

namespace mdrivlib
{

static void ISRHandler(unsigned irqnum) {
	InterruptManager::callISR(irqnum);
}

// clang-format off
extern "C" {
void IRQ_Trampoline_0() { ISRHandler(0); }

void IRQ_Trampoline_2() { ISRHandler(2); }
void IRQ_Trampoline_3() { ISRHandler(3); }
void IRQ_Trampoline_4() { ISRHandler(4); }
void IRQ_Trampoline_5() { ISRHandler(5); }
void IRQ_Trampoline_6() { ISRHandler(6); }
void IRQ_Trampoline_7() { ISRHandler(7); }

void IRQ_Trampoline_9() { ISRHandler(9); }
void IRQ_Trampoline_10() { ISRHandler(10); }
void IRQ_Trampoline_11() { ISRHandler(11); }
void IRQ_Trampoline_12() { ISRHandler(12); }
void IRQ_Trampoline_13() { ISRHandler(13); }
void IRQ_Trampoline_14() { ISRHandler(14); }

void IRQ_Trampoline_16() { ISRHandler(16); }


void IRQ_Trampoline_19() { ISRHandler(19); }

void IRQ_Trampoline_21() { ISRHandler(21); }
void IRQ_Trampoline_22() { ISRHandler(22); }
void IRQ_Trampoline_23() { ISRHandler(23); }

void IRQ_Trampoline_25() { ISRHandler(25); }

void IRQ_Trampoline_27() { ISRHandler(27); }
// clang-format on
}

} // namespace mdrivlib
