#include "interrupt.hh"

static void ISRHandler(IRQType irqnum) { InterruptManager::callISR(irqnum); }

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
