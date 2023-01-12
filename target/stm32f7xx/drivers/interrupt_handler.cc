#include "interrupt.hh"

namespace mdrivlib
{

static void ISRHandler(unsigned irqnum) {
	InterruptManager::callISR(irqnum);
}

// clang-format off
extern "C" {
void IRQ_Trampoline_0() { ISRHandler(0); }
void IRQ_Trampoline_1() { ISRHandler(1); }
void IRQ_Trampoline_2() { ISRHandler(2); }
void IRQ_Trampoline_3() { ISRHandler(3); }
void IRQ_Trampoline_4() { ISRHandler(4); }
void IRQ_Trampoline_5() { ISRHandler(5); }
void IRQ_Trampoline_6() { ISRHandler(6); }
void IRQ_Trampoline_7() { ISRHandler(7); }
void IRQ_Trampoline_8() { ISRHandler(8); }
void IRQ_Trampoline_9() { ISRHandler(9); }
void IRQ_Trampoline_10() { ISRHandler(10); }
void IRQ_Trampoline_11() { ISRHandler(11); }
void IRQ_Trampoline_12() { ISRHandler(12); }
void IRQ_Trampoline_13() { ISRHandler(13); }
void IRQ_Trampoline_14() { ISRHandler(14); }
void IRQ_Trampoline_15() { ISRHandler(15); }
void IRQ_Trampoline_16() { ISRHandler(16); }
void IRQ_Trampoline_17() { ISRHandler(17); }
void IRQ_Trampoline_18() { ISRHandler(18); }
void IRQ_Trampoline_19() { ISRHandler(19); }
void IRQ_Trampoline_20() { ISRHandler(20); }
void IRQ_Trampoline_21() { ISRHandler(21); }
void IRQ_Trampoline_22() { ISRHandler(22); }
void IRQ_Trampoline_23() { ISRHandler(23); }
void IRQ_Trampoline_24() { ISRHandler(24); }
void IRQ_Trampoline_25() { ISRHandler(25); }
void IRQ_Trampoline_26() { ISRHandler(26); }
void IRQ_Trampoline_27() { ISRHandler(27); }
void IRQ_Trampoline_28() { ISRHandler(28); }
void IRQ_Trampoline_29() { ISRHandler(29); }
void IRQ_Trampoline_30() { ISRHandler(30); }
void IRQ_Trampoline_31() { ISRHandler(31); }
void IRQ_Trampoline_32() { ISRHandler(32); }
void IRQ_Trampoline_33() { ISRHandler(33); }
void IRQ_Trampoline_34() { ISRHandler(34); }
void IRQ_Trampoline_35() { ISRHandler(35); }
void IRQ_Trampoline_36() { ISRHandler(36); }
void IRQ_Trampoline_37() { ISRHandler(37); }
void IRQ_Trampoline_38() { ISRHandler(38); }
void IRQ_Trampoline_39() { ISRHandler(39); }
void IRQ_Trampoline_40() { ISRHandler(40); }
void IRQ_Trampoline_41() { ISRHandler(41); }
void IRQ_Trampoline_42() { ISRHandler(42); }
void IRQ_Trampoline_43() { ISRHandler(43); }
void IRQ_Trampoline_44() { ISRHandler(44); }
void IRQ_Trampoline_45() { ISRHandler(45); }
void IRQ_Trampoline_46() { ISRHandler(46); }
void IRQ_Trampoline_47() { ISRHandler(47); }
void IRQ_Trampoline_48() { ISRHandler(48); }
void IRQ_Trampoline_49() { ISRHandler(49); }
void IRQ_Trampoline_50() { ISRHandler(50); }
void IRQ_Trampoline_51() { ISRHandler(51); }
void IRQ_Trampoline_52() { ISRHandler(52); }
void IRQ_Trampoline_53() { ISRHandler(53); }
void IRQ_Trampoline_54() { ISRHandler(54); }
void IRQ_Trampoline_55() { ISRHandler(55); }
void IRQ_Trampoline_56() { ISRHandler(56); }
void IRQ_Trampoline_57() { ISRHandler(57); }
void IRQ_Trampoline_58() { ISRHandler(58); }
void IRQ_Trampoline_59() { ISRHandler(59); }
void IRQ_Trampoline_60() { ISRHandler(60); }
void IRQ_Trampoline_61() { ISRHandler(61); }
void IRQ_Trampoline_62() { ISRHandler(62); }
void IRQ_Trampoline_63() { ISRHandler(63); }
void IRQ_Trampoline_64() { ISRHandler(64); }
void IRQ_Trampoline_65() { ISRHandler(65); }
void IRQ_Trampoline_66() { ISRHandler(66); }
void IRQ_Trampoline_67() { ISRHandler(67); }
void IRQ_Trampoline_68() { ISRHandler(68); }
void IRQ_Trampoline_69() { ISRHandler(69); }
void IRQ_Trampoline_70() { ISRHandler(70); }
void IRQ_Trampoline_71() { ISRHandler(71); }
void IRQ_Trampoline_72() { ISRHandler(72); }
void IRQ_Trampoline_73() { ISRHandler(73); }
void IRQ_Trampoline_74() { ISRHandler(74); }
void IRQ_Trampoline_75() { ISRHandler(75); }
void IRQ_Trampoline_76() { ISRHandler(76); }
void IRQ_Trampoline_77() { ISRHandler(77); }
void IRQ_Trampoline_78() { ISRHandler(78); }
void IRQ_Trampoline_79() { ISRHandler(79); }
void IRQ_Trampoline_80() { ISRHandler(80); }
void IRQ_Trampoline_81() { ISRHandler(81); }
void IRQ_Trampoline_82() { ISRHandler(82); }
void IRQ_Trampoline_83() { ISRHandler(83); }
void IRQ_Trampoline_84() { ISRHandler(84); }
void IRQ_Trampoline_85() { ISRHandler(85); }
void IRQ_Trampoline_86() { ISRHandler(86); }
void IRQ_Trampoline_87() { ISRHandler(87); }
void IRQ_Trampoline_88() { ISRHandler(88); }
void IRQ_Trampoline_89() { ISRHandler(89); }
void IRQ_Trampoline_90() { ISRHandler(90); }
void IRQ_Trampoline_91() { ISRHandler(91); }
void IRQ_Trampoline_92() { ISRHandler(92); }
void IRQ_Trampoline_93() { ISRHandler(93); }
void IRQ_Trampoline_94() { ISRHandler(94); }
void IRQ_Trampoline_95() { ISRHandler(95); }
void IRQ_Trampoline_96() { ISRHandler(96); }
void IRQ_Trampoline_97() { ISRHandler(97); }

void IRQ_Trampoline_98() { ISRHandler(98); }
void IRQ_Trampoline_99() { ISRHandler(99); }
void IRQ_Trampoline_100() { ISRHandler(100); }
void IRQ_Trampoline_101() { ISRHandler(101); }
void IRQ_Trampoline_102() { ISRHandler(102); }
void IRQ_Trampoline_103() { ISRHandler(103); }
// clang-format on
}

} // namespace mdrivlib
