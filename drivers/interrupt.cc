#include "interrupt.hh"

// static void ISRHandler(IRQType irqnum) { InterruptManager::callISR(irqnum); }
static void ISRHandler(unsigned irqnum) { InterruptManager::callISR(irqnum); }

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
void IRQ_Trampoline_104() { ISRHandler(104); }
void IRQ_Trampoline_105() { ISRHandler(105); }
void IRQ_Trampoline_106() { ISRHandler(106); }
void IRQ_Trampoline_107() { ISRHandler(107); }
void IRQ_Trampoline_108() { ISRHandler(108); }
void IRQ_Trampoline_109() { ISRHandler(109); }
void IRQ_Trampoline_110() { ISRHandler(110); }
void IRQ_Trampoline_111() { ISRHandler(111); }
void IRQ_Trampoline_112() { ISRHandler(112); }
void IRQ_Trampoline_113() { ISRHandler(113); }
void IRQ_Trampoline_114() { ISRHandler(114); }
void IRQ_Trampoline_115() { ISRHandler(115); }
void IRQ_Trampoline_116() { ISRHandler(116); }
void IRQ_Trampoline_117() { ISRHandler(117); }
void IRQ_Trampoline_118() { ISRHandler(118); }
void IRQ_Trampoline_119() { ISRHandler(119); }
void IRQ_Trampoline_120() { ISRHandler(120); }
void IRQ_Trampoline_121() { ISRHandler(121); }
void IRQ_Trampoline_122() { ISRHandler(122); }
void IRQ_Trampoline_123() { ISRHandler(123); }
void IRQ_Trampoline_124() { ISRHandler(124); }
void IRQ_Trampoline_125() { ISRHandler(125); }
void IRQ_Trampoline_126() { ISRHandler(126); }
void IRQ_Trampoline_127() { ISRHandler(127); }
void IRQ_Trampoline_128() { ISRHandler(128); }
void IRQ_Trampoline_129() { ISRHandler(129); }
void IRQ_Trampoline_130() { ISRHandler(130); }
void IRQ_Trampoline_131() { ISRHandler(131); }
void IRQ_Trampoline_132() { ISRHandler(132); }
void IRQ_Trampoline_133() { ISRHandler(133); }
void IRQ_Trampoline_134() { ISRHandler(134); }
void IRQ_Trampoline_135() { ISRHandler(135); }
void IRQ_Trampoline_136() { ISRHandler(136); }
void IRQ_Trampoline_137() { ISRHandler(137); }
void IRQ_Trampoline_138() { ISRHandler(138); }
void IRQ_Trampoline_139() { ISRHandler(139); }
void IRQ_Trampoline_140() { ISRHandler(140); }
void IRQ_Trampoline_141() { ISRHandler(141); }
void IRQ_Trampoline_142() { ISRHandler(142); }
void IRQ_Trampoline_143() { ISRHandler(143); }
void IRQ_Trampoline_144() { ISRHandler(144); }
void IRQ_Trampoline_145() { ISRHandler(145); }
void IRQ_Trampoline_146() { ISRHandler(146); }
void IRQ_Trampoline_147() { ISRHandler(147); }
void IRQ_Trampoline_148() { ISRHandler(148); }
void IRQ_Trampoline_149() { ISRHandler(149); }
void IRQ_Trampoline_150() { ISRHandler(150); }
void IRQ_Trampoline_151() { ISRHandler(151); }
void IRQ_Trampoline_152() { ISRHandler(152); }
void IRQ_Trampoline_153() { ISRHandler(153); }
void IRQ_Trampoline_154() { ISRHandler(154); }
void IRQ_Trampoline_155() { ISRHandler(155); }
void IRQ_Trampoline_156() { ISRHandler(156); }
void IRQ_Trampoline_157() { ISRHandler(157); }
void IRQ_Trampoline_158() { ISRHandler(158); }
void IRQ_Trampoline_159() { ISRHandler(159); }
void IRQ_Trampoline_160() { ISRHandler(160); }
void IRQ_Trampoline_161() { ISRHandler(161); }
void IRQ_Trampoline_162() { ISRHandler(162); }
void IRQ_Trampoline_163() { ISRHandler(163); }
void IRQ_Trampoline_164() { ISRHandler(164); }
void IRQ_Trampoline_165() { ISRHandler(165); }
void IRQ_Trampoline_166() { ISRHandler(166); }
void IRQ_Trampoline_167() { ISRHandler(167); }
void IRQ_Trampoline_168() { ISRHandler(168); }
void IRQ_Trampoline_169() { ISRHandler(169); }
void IRQ_Trampoline_170() { ISRHandler(170); }
void IRQ_Trampoline_171() { ISRHandler(171); }
void IRQ_Trampoline_172() { ISRHandler(172); }
void IRQ_Trampoline_173() { ISRHandler(173); }
void IRQ_Trampoline_174() { ISRHandler(174); }
void IRQ_Trampoline_175() { ISRHandler(175); }
void IRQ_Trampoline_176() { ISRHandler(176); }
void IRQ_Trampoline_177() { ISRHandler(177); }
void IRQ_Trampoline_178() { ISRHandler(178); }
void IRQ_Trampoline_179() { ISRHandler(179); }
void IRQ_Trampoline_180() { ISRHandler(180); }
void IRQ_Trampoline_181() { ISRHandler(181); }
void IRQ_Trampoline_182() { ISRHandler(182); }
void IRQ_Trampoline_183() { ISRHandler(183); }
void IRQ_Trampoline_184() { ISRHandler(184); }
void IRQ_Trampoline_185() { ISRHandler(185); }
void IRQ_Trampoline_186() { ISRHandler(186); }
void IRQ_Trampoline_187() { ISRHandler(187); }
void IRQ_Trampoline_188() { ISRHandler(188); }
void IRQ_Trampoline_189() { ISRHandler(189); }
void IRQ_Trampoline_190() { ISRHandler(190); }
void IRQ_Trampoline_191() { ISRHandler(191); }
void IRQ_Trampoline_192() { ISRHandler(192); }
void IRQ_Trampoline_193() { ISRHandler(193); }
void IRQ_Trampoline_194() { ISRHandler(194); }
void IRQ_Trampoline_195() { ISRHandler(195); }
void IRQ_Trampoline_196() { ISRHandler(196); }
void IRQ_Trampoline_197() { ISRHandler(197); }
void IRQ_Trampoline_198() { ISRHandler(198); }
void IRQ_Trampoline_199() { ISRHandler(199); }
void IRQ_Trampoline_200() { ISRHandler(200); }
void IRQ_Trampoline_201() { ISRHandler(201); }
void IRQ_Trampoline_202() { ISRHandler(202); }
void IRQ_Trampoline_203() { ISRHandler(203); }
void IRQ_Trampoline_204() { ISRHandler(204); }
void IRQ_Trampoline_205() { ISRHandler(205); }
void IRQ_Trampoline_206() { ISRHandler(206); }
void IRQ_Trampoline_207() { ISRHandler(207); }
void IRQ_Trampoline_208() { ISRHandler(208); }
void IRQ_Trampoline_209() { ISRHandler(209); }
void IRQ_Trampoline_210() { ISRHandler(210); }
void IRQ_Trampoline_211() { ISRHandler(211); }
void IRQ_Trampoline_212() { ISRHandler(212); }
void IRQ_Trampoline_213() { ISRHandler(213); }
void IRQ_Trampoline_214() { ISRHandler(214); }
void IRQ_Trampoline_215() { ISRHandler(215); }
void IRQ_Trampoline_216() { ISRHandler(216); }
void IRQ_Trampoline_217() { ISRHandler(217); }
void IRQ_Trampoline_218() { ISRHandler(218); }
void IRQ_Trampoline_219() { ISRHandler(219); }
void IRQ_Trampoline_220() { ISRHandler(220); }
void IRQ_Trampoline_221() { ISRHandler(221); }
void IRQ_Trampoline_222() { ISRHandler(222); }
void IRQ_Trampoline_223() { ISRHandler(223); }
void IRQ_Trampoline_224() { ISRHandler(224); }
void IRQ_Trampoline_225() { ISRHandler(225); }
void IRQ_Trampoline_226() { ISRHandler(226); }
void IRQ_Trampoline_227() { ISRHandler(227); }
void IRQ_Trampoline_228() { ISRHandler(228); }
void IRQ_Trampoline_229() { ISRHandler(229); }
void IRQ_Trampoline_230() { ISRHandler(230); }
void IRQ_Trampoline_231() { ISRHandler(231); }
void IRQ_Trampoline_232() { ISRHandler(232); }
void IRQ_Trampoline_233() { ISRHandler(233); }
void IRQ_Trampoline_234() { ISRHandler(234); }
void IRQ_Trampoline_235() { ISRHandler(235); }
void IRQ_Trampoline_236() { ISRHandler(236); }
void IRQ_Trampoline_237() { ISRHandler(237); }
void IRQ_Trampoline_238() { ISRHandler(238); }
void IRQ_Trampoline_239() { ISRHandler(239); }
void IRQ_Trampoline_240() { ISRHandler(240); }
void IRQ_Trampoline_241() { ISRHandler(241); }
void IRQ_Trampoline_242() { ISRHandler(242); }
void IRQ_Trampoline_243() { ISRHandler(243); }
void IRQ_Trampoline_244() { ISRHandler(244); }
void IRQ_Trampoline_245() { ISRHandler(245); }
void IRQ_Trampoline_246() { ISRHandler(246); }
void IRQ_Trampoline_247() { ISRHandler(247); }
void IRQ_Trampoline_248() { ISRHandler(248); }
void IRQ_Trampoline_249() { ISRHandler(249); }
void IRQ_Trampoline_250() { ISRHandler(250); }
void IRQ_Trampoline_251() { ISRHandler(251); }
void IRQ_Trampoline_252() { ISRHandler(252); }
void IRQ_Trampoline_253() { ISRHandler(253); }
void IRQ_Trampoline_254() { ISRHandler(254); }
void IRQ_Trampoline_255() { ISRHandler(255); }
}
/*
extern "C" void WWDG_IRQHandler(void) { ISRHandler(WWDG_IRQn); }
extern "C" void PVD_IRQHandler(void) { ISRHandler(PVD_IRQn); }
extern "C" void TAMP_STAMP_IRQHandler(void) { ISRHandler(TAMP_STAMP_IRQn); }
extern "C" void RTC_WKUP_IRQHandler(void) { ISRHandler(RTC_WKUP_IRQn); }
extern "C" void FLASH_IRQHandler(void) { ISRHandler(FLASH_IRQn); }
extern "C" void RCC_IRQHandler(void) { ISRHandler(RCC_IRQn); }
extern "C" void EXTI0_IRQHandler(void) { ISRHandler(EXTI0_IRQn); }
extern "C" void EXTI1_IRQHandler(void) { ISRHandler(EXTI1_IRQn); }
extern "C" void EXTI2_IRQHandler(void) { ISRHandler(EXTI2_IRQn); }
extern "C" void EXTI3_IRQHandler(void) { ISRHandler(EXTI3_IRQn); }
extern "C" void EXTI4_IRQHandler(void) { ISRHandler(EXTI4_IRQn); }
extern "C" void DMA1_Stream0_IRQHandler(void) { ISRHandler(DMA1_Stream0_IRQn); }
extern "C" void DMA1_Stream1_IRQHandler(void) { ISRHandler(DMA1_Stream1_IRQn); }
extern "C" void DMA1_Stream2_IRQHandler(void) { ISRHandler(DMA1_Stream2_IRQn); }
extern "C" void DMA1_Stream3_IRQHandler(void) { ISRHandler(DMA1_Stream3_IRQn); }
extern "C" void DMA1_Stream4_IRQHandler(void) { ISRHandler(DMA1_Stream4_IRQn); }
extern "C" void DMA1_Stream5_IRQHandler(void) { ISRHandler(DMA1_Stream5_IRQn); }
extern "C" void DMA1_Stream6_IRQHandler(void) { ISRHandler(DMA1_Stream6_IRQn); }
extern "C" void ADC_IRQHandler(void) { ISRHandler(ADC_IRQn); }
#ifdef CAN1
extern "C" void CAN1_TX_IRQHandler(void) { ISRHandler(CAN1_TX_IRQn); }
extern "C" void CAN1_RX0_IRQHandler(void) { ISRHandler(CAN1_RX0_IRQn); }
extern "C" void CAN1_RX1_IRQHandler(void) { ISRHandler(CAN1_RX1_IRQn); }
extern "C" void CAN1_SCE_IRQHandler(void) { ISRHandler(CAN1_SCE_IRQn); }
#endif
#ifdef FDCAN
extern "C" void FDCAN1_IT0_IRQHandler(void) { ISRHandler(FDCAN1_IT0_IRQn); }
extern "C" void FDCAN2_IT0_IRQHandler(void) { ISRHandler(FDCAN2_IT0_IRQn); }
extern "C" void FDCAN1_IT1_IRQHandler(void) { ISRHandler(FDCAN1_IT1_IRQn); }
extern "C" void FDCAN2_IT1_IRQHandler(void) { ISRHandler(FDCAN2_IT1_IRQn); }
#endif
extern "C" void EXTI9_5_IRQHandler(void) { ISRHandler(EXTI9_5_IRQn); }
extern "C" void TIM1_BRK_TIM9_IRQHandler(void) { ISRHandler(TIM1_BRK_TIM9_IRQn); }
extern "C" void TIM1_UP_TIM10_IRQHandler(void) { ISRHandler(TIM1_UP_TIM10_IRQn); }
extern "C" void TIM1_TRG_COM_TIM11_IRQHandler(void) { ISRHandler(TIM1_TRG_COM_TIM11_IRQn); }
extern "C" void TIM1_CC_IRQHandler(void) { ISRHandler(TIM1_CC_IRQn); }
extern "C" void TIM2_IRQHandler(void) { ISRHandler(TIM2_IRQn); }
extern "C" void TIM3_IRQHandler(void) { ISRHandler(TIM3_IRQn); }
extern "C" void TIM4_IRQHandler(void) { ISRHandler(TIM4_IRQn); }
extern "C" void I2C1_EV_IRQHandler(void) { ISRHandler(I2C1_EV_IRQn); }
extern "C" void I2C1_ER_IRQHandler(void) { ISRHandler(I2C1_ER_IRQn); }
extern "C" void I2C2_EV_IRQHandler(void) { ISRHandler(I2C2_EV_IRQn); }
extern "C" void I2C2_ER_IRQHandler(void) { ISRHandler(I2C2_ER_IRQn); }
extern "C" void SPI1_IRQHandler(void) { ISRHandler(SPI1_IRQn); }
extern "C" void SPI2_IRQHandler(void) { ISRHandler(SPI2_IRQn); }
extern "C" void USART1_IRQHandler(void) { ISRHandler(USART1_IRQn); }
extern "C" void USART2_IRQHandler(void) { ISRHandler(USART2_IRQn); }
extern "C" void USART3_IRQHandler(void) { ISRHandler(USART3_IRQn); }
extern "C" void EXTI15_10_IRQHandler(void) { ISRHandler(EXTI15_10_IRQn); }
extern "C" void RTC_Alarm_IRQHandler(void) { ISRHandler(RTC_Alarm_IRQn); }
extern "C" void OTG_FS_WKUP_IRQHandler(void) { ISRHandler(OTG_FS_WKUP_IRQn); }
extern "C" void TIM8_BRK_TIM12_IRQHandler(void) { ISRHandler(TIM8_BRK_TIM12_IRQn); }
extern "C" void TIM8_UP_TIM13_IRQHandler(void) { ISRHandler(TIM8_UP_TIM13_IRQn); }
extern "C" void TIM8_TRG_COM_TIM14_IRQHandler(void) { ISRHandler(TIM8_TRG_COM_TIM14_IRQn); }
extern "C" void TIM8_CC_IRQHandler(void) { ISRHandler(TIM8_CC_IRQn); }
extern "C" void DMA1_Stream7_IRQHandler(void) { ISRHandler(DMA1_Stream7_IRQn); }
extern "C" void FMC_IRQHandler(void) { ISRHandler(FMC_IRQn); }
extern "C" void SDMMC1_IRQHandler(void) { ISRHandler(SDMMC1_IRQn); }
extern "C" void TIM5_IRQHandler(void) { ISRHandler(TIM5_IRQn); }
extern "C" void SPI3_IRQHandler(void) { ISRHandler(SPI3_IRQn); }
extern "C" void UART4_IRQHandler(void) { ISRHandler(UART4_IRQn); }
extern "C" void UART5_IRQHandler(void) { ISRHandler(UART5_IRQn); }
extern "C" void TIM6_DAC_IRQHandler(void) { ISRHandler(TIM6_DAC_IRQn); }
extern "C" void TIM7_IRQHandler(void) { ISRHandler(TIM7_IRQn); }
extern "C" void DMA2_Stream0_IRQHandler(void) { ISRHandler(DMA2_Stream0_IRQn); }
extern "C" void DMA2_Stream1_IRQHandler(void) { ISRHandler(DMA2_Stream1_IRQn); }
extern "C" void DMA2_Stream2_IRQHandler(void) { ISRHandler(DMA2_Stream2_IRQn); }
extern "C" void DMA2_Stream3_IRQHandler(void) { ISRHandler(DMA2_Stream3_IRQn); }
extern "C" void DMA2_Stream4_IRQHandler(void) { ISRHandler(DMA2_Stream4_IRQn); }
extern "C" void OTG_FS_IRQHandler(void) { ISRHandler(OTG_FS_IRQn); }
#ifdef CM4_SEV_IRQn
extern "C" void CM4_SEV_IRQHandler(void) { ISRHandler(CM4_SEV_IRQn); }
#endif
#ifdef CM7_SEV_IRQn
extern "C" void CM7_SEV_IRQHandler(void) { ISRHandler(CM7_SEV_IRQn); }
#endif
extern "C" void DMA2_Stream5_IRQHandler(void) { ISRHandler(DMA2_Stream5_IRQn); }
extern "C" void DMA2_Stream6_IRQHandler(void) { ISRHandler(DMA2_Stream6_IRQn); }
extern "C" void DMA2_Stream7_IRQHandler(void) { ISRHandler(DMA2_Stream7_IRQn); }
extern "C" void USART6_IRQHandler(void) { ISRHandler(USART6_IRQn); }
extern "C" void I2C3_EV_IRQHandler(void) { ISRHandler(I2C3_EV_IRQn); }
extern "C" void I2C3_ER_IRQHandler(void) { ISRHandler(I2C3_ER_IRQn); }
extern "C" void OTG_HS_EP1_OUT_IRQHandler(void) { ISRHandler(OTG_HS_EP1_OUT_IRQn); }
extern "C" void OTG_HS_EP1_IN_IRQHandler(void) { ISRHandler(OTG_HS_EP1_IN_IRQn); }
extern "C" void OTG_HS_WKUP_IRQHandler(void) { ISRHandler(OTG_HS_WKUP_IRQn); }
extern "C" void OTG_HS_IRQHandler(void) { ISRHandler(OTG_HS_IRQn); }
#ifdef AES
extern "C" void AES_IRQHandler(void) { ISRHandler(AES_IRQn); }
#endif
extern "C" void RNG_IRQHandler(void) { ISRHandler(RNG_IRQn); }
extern "C" void FPU_IRQHandler(void) { ISRHandler(FPU_IRQn); }
extern "C" void UART7_IRQHandler(void) { ISRHandler(UART7_IRQn); }
extern "C" void UART8_IRQHandler(void) { ISRHandler(UART8_IRQn); }
extern "C" void SPI4_IRQHandler(void) { ISRHandler(SPI4_IRQn); }
extern "C" void SPI5_IRQHandler(void) { ISRHandler(SPI5_IRQn); }
extern "C" void SAI1_IRQHandler(void) { ISRHandler(SAI1_IRQn); }
extern "C" void SAI2_IRQHandler(void) { ISRHandler(SAI2_IRQn); }
extern "C" void QUADSPI_IRQHandler(void) { ISRHandler(QUADSPI_IRQn); }
extern "C" void LPTIM1_IRQHandler(void) { ISRHandler(LPTIM1_IRQn); }
#ifdef SDMMC2
extern "C" void SDMMC2_IRQHandler(void) { ISRHandler(SDMMC2_IRQn); }
#endif
#ifdef SPI6
extern "C" void SPI6_IRQHandler(void) { ISRHandler(SPI6_IRQn); }
#endif
#ifdef LTDC
extern "C" void LTDC_IRQHandler(void) { ISRHandler(LTDC_IRQn); }
#endif
#ifdef LTDC
extern "C" void LTDC_ER_IRQHandler(void) { ISRHandler(LTDC_ER_IRQn); }
#endif
#ifdef DMA2D
extern "C" void DMA2D_IRQHandler(void) { ISRHandler(DMA2D_IRQn); }
#endif
#ifdef CEC
extern "C" void CEC_IRQHandler(void) { ISRHandler(CEC_IRQn); }
#endif
#ifdef I2C4
extern "C" void I2C4_EV_IRQHandler(void) { ISRHandler(I2C4_EV_IRQn); }
#endif
#ifdef I2C4
extern "C" void I2C4_ER_IRQHandler(void) { ISRHandler(I2C4_ER_IRQn); }
#endif
#ifdef SPDIF_RX
extern "C" void SPDIF_RX_IRQHandler(void) { ISRHandler(SPDIF_RX_IRQn); }
#endif

#ifdef OTG_FS_EP1_OUT_IRQn
extern "C" void OTG_FS_EP1_OUT_IRQHandler(void) { ISRHandler(OTG_FS_EP1_OUT_IRQn); }
#endif
#ifdef OTG_FS_EP1_IN_IRQn
extern "C" void OTG_FS_EP1_IN_IRQHandler(void) { ISRHandler(OTG_FS_EP1_IN_IRQn); }
#endif
#ifdef OTG_FS_WKUP_IRQn
extern "C" void OTG_FS_WKUP_IRQHandler(void) { ISRHandler(OTG_FS_WKUP_IRQn); }
#endif
#ifdef OTG_FS_IRQn
extern "C" void OTG_FS_IRQHandler(void) { ISRHandler(OTG_FS_IRQn); }
#endif
#ifdef DMAMUX1_OVR_IRQn
extern "C" void DMAMUX1_OVR_IRQHandler(void) { ISRHandler(DMAMUX1_OVR_IRQn); }
#endif
#ifdef HRTIM1_Master_IRQn
extern "C" void HRTIM1_Master_IRQHandler(void) { ISRHandler(HRTIM1_Master_IRQn); }
#endif
#ifdef HRTIM1_TIMA_IRQn
extern "C" void HRTIM1_TIMA_IRQHandler(void) { ISRHandler(HRTIM1_TIMA_IRQn); }
#endif
#ifdef HRTIM1_TIMB_IRQn
extern "C" void HRTIM1_TIMB_IRQHandler(void) { ISRHandler(HRTIM1_TIMB_IRQn); }
#endif
#ifdef HRTIM1_TIMC_IRQn
extern "C" void HRTIM1_TIMC_IRQHandler(void) { ISRHandler(HRTIM1_TIMC_IRQn); }
#endif
#ifdef HRTIM1_TIMD_IRQn
extern "C" void HRTIM1_TIMD_IRQHandler(void) { ISRHandler(HRTIM1_TIMD_IRQn); }
#endif
#ifdef HRTIM1_TIME_IRQn
extern "C" void HRTIM1_TIME_IRQHandler(void) { ISRHandler(HRTIM1_TIME_IRQn); }
#endif
#ifdef HRTIM1_FLT_IRQn
extern "C" void HRTIM1_FLT_IRQHandler(void) { ISRHandler(HRTIM1_FLT_IRQn); }
#endif
#ifdef DFSDM1_FLT0_IRQn
extern "C" void DFSDM1_FLT0_IRQHandler(void) { ISRHandler(DFSDM1_FLT0_IRQn); }
#endif
#ifdef DFSDM1_FLT1_IRQn
extern "C" void DFSDM1_FLT1_IRQHandler(void) { ISRHandler(DFSDM1_FLT1_IRQn); }
#endif
#ifdef DFSDM1_FLT2_IRQn
extern "C" void DFSDM1_FLT2_IRQHandler(void) { ISRHandler(DFSDM1_FLT2_IRQn); }
#endif
#ifdef DFSDM1_FLT3_IRQn
extern "C" void DFSDM1_FLT3_IRQHandler(void) { ISRHandler(DFSDM1_FLT3_IRQn); }
#endif
#ifdef SAI3_IRQn
extern "C" void SAI3_IRQHandler(void) { ISRHandler(SAI3_IRQn); }
#endif
#ifdef SWPMI1_IRQn
extern "C" void SWPMI1_IRQHandler(void) { ISRHandler(SWPMI1_IRQn); }
#endif
#ifdef TIM15_IRQn
extern "C" void TIM15_IRQHandler(void) { ISRHandler(TIM15_IRQn); }
#endif
#ifdef TIM16_IRQn
extern "C" void TIM16_IRQHandler(void) { ISRHandler(TIM16_IRQn); }
#endif
#ifdef TIM17_IRQn
extern "C" void TIM17_IRQHandler(void) { ISRHandler(TIM17_IRQn); }
#endif
#ifdef MDIOS_WKUP_IRQn
extern "C" void MDIOS_WKUP_IRQHandler(void) { ISRHandler(MDIOS_WKUP_IRQn); }
#endif
#ifdef MDIOS_IRQn
extern "C" void MDIOS_IRQHandler(void) { ISRHandler(MDIOS_IRQn); }
#endif
#ifdef JPEG_IRQn
extern "C" void JPEG_IRQHandler(void) { ISRHandler(JPEG_IRQn); }
#endif
#ifdef MDMA_IRQn
extern "C" void MDMA_IRQHandler(void) { ISRHandler(MDMA_IRQn); }
#endif
#ifdef HSEM1_IRQn
extern "C" void HSEM1_IRQHandler(void) { ISRHandler(HSEM1_IRQn); }
#endif
#ifdef HSEM2_IRQn
extern "C" void HSEM2_IRQHandler(void) { ISRHandler(HSEM2_IRQn); }
#endif
#ifdef ADC3_IRQn
extern "C" void ADC3_IRQHandler(void) { ISRHandler(ADC3_IRQn); }
#endif
#ifdef DMAMUX2_OVR_IRQn
extern "C" void DMAMUX2_OVR_IRQHandler(void) { ISRHandler(DMAMUX2_OVR_IRQn); }
#endif
#ifdef BDMA_Channel0_IRQn
extern "C" void BDMA_Channel0_IRQHandler(void) { ISRHandler(BDMA_Channel0_IRQn); }
#endif
#ifdef BDMA_Channel1_IRQn
extern "C" void BDMA_Channel1_IRQHandler(void) { ISRHandler(BDMA_Channel1_IRQn); }
#endif
#ifdef BDMA_Channel2_IRQn
extern "C" void BDMA_Channel2_IRQHandler(void) { ISRHandler(BDMA_Channel2_IRQn); }
#endif
#ifdef BDMA_Channel3_IRQn
extern "C" void BDMA_Channel3_IRQHandler(void) { ISRHandler(BDMA_Channel3_IRQn); }
#endif
#ifdef BDMA_Channel4_IRQn
extern "C" void BDMA_Channel4_IRQHandler(void) { ISRHandler(BDMA_Channel4_IRQn); }
#endif
#ifdef BDMA_Channel5_IRQn
extern "C" void BDMA_Channel5_IRQHandler(void) { ISRHandler(BDMA_Channel5_IRQn); }
#endif
#ifdef BDMA_Channel6_IRQn
extern "C" void BDMA_Channel6_IRQHandler(void) { ISRHandler(BDMA_Channel6_IRQn); }
#endif
#ifdef BDMA_Channel7_IRQn
extern "C" void BDMA_Channel7_IRQHandler(void) { ISRHandler(BDMA_Channel7_IRQn); }
#endif
#ifdef COMP1_IRQn
extern "C" void COMP1_IRQHandler(void) { ISRHandler(COMP1_IRQn); }
#endif
#ifdef LPTIM2_IRQn
extern "C" void LPTIM2_IRQHandler(void) { ISRHandler(LPTIM2_IRQn); }
#endif
#ifdef LPTIM3_IRQn
extern "C" void LPTIM3_IRQHandler(void) { ISRHandler(LPTIM3_IRQn); }
#endif
#ifdef LPTIM4_IRQn
extern "C" void LPTIM4_IRQHandler(void) { ISRHandler(LPTIM4_IRQn); }
#endif
#ifdef LPTIM5_IRQn
extern "C" void LPTIM5_IRQHandler(void) { ISRHandler(LPTIM5_IRQn); }
#endif
#ifdef LPUART1_IRQn
extern "C" void LPUART1_IRQHandler(void) { ISRHandler(LPUART1_IRQn); }
#endif
#ifdef WWDG_RST_IRQn
extern "C" void WWDG_RST_IRQHandler(void) { ISRHandler(WWDG_RST_IRQn); }
#endif
#ifdef CRS_IRQn
extern "C" void CRS_IRQHandler(void) { ISRHandler(CRS_IRQn); }
#endif
#ifdef ECC_IRQn
extern "C" void ECC_IRQHandler(void) { ISRHandler(ECC_IRQn); }
#endif
#ifdef SAI4_IRQn
extern "C" void SAI4_IRQHandler(void) { ISRHandler(SAI4_IRQn); }
#endif
#ifdef HOLD_CORE_IRQn
extern "C" void HOLD_CORE_IRQHandler(void) { ISRHandler(HOLD_CORE_IRQn); }
#endif
#ifdef WAKEUP_PIN_IRQn
extern "C" void WAKEUP_PIN_IRQHandler(void) { ISRHandler(WAKEUP_PIN_IRQn); }
#endif
*/
