#pragma once
#include "drivers/register_access.hh"
#ifndef STM32MP13
#define STM32MP13
#endif
#include "drivers/stm32xx.h"

namespace mdrivlib
{

namespace RCC_Reset_Set
{
// clang-format off

// Auto-generated:
// %s/\v#define RCC_(.{-}RSTSETR)_([^ ]{-})RST[^_] /using \2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, \1), RCC_\1_\2RST>;\r\0/

using ADC1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTSETR), RCC_AHB2RSTSETR_ADC1RST>;
using ADC2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTSETR), RCC_AHB2RSTSETR_ADC2RST>;
using AXIMC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB5RSTSETR), RCC_AHB5RSTSETR_AXIMCRST>;
using CRC1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTSETR), RCC_AHB6RSTSETR_CRC1RST>;
using CRYP1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB5RSTSETR), RCC_AHB5RSTSETR_CRYP1RST>;
using DCMIPP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4RSTSETR), RCC_APB4RSTSETR_DCMIPPRST>;
using DDRPERFM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4RSTSETR), RCC_APB4RSTSETR_DDRPERFMRST>;
using DFSDM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTSETR), RCC_APB2RSTSETR_DFSDMRST>;
using DMA1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTSETR), RCC_AHB2RSTSETR_DMA1RST>;
using DMA2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTSETR), RCC_AHB2RSTSETR_DMA2RST>;
using DMA3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTSETR), RCC_AHB2RSTSETR_DMA3RST>;
using DMAMUX1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTSETR), RCC_AHB2RSTSETR_DMAMUX1RST>;
using DMAMUX2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTSETR), RCC_AHB2RSTSETR_DMAMUX2RST>;
using DTS_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTSETR), RCC_APB3RSTSETR_DTSRST>;
using ETH1MAC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTSETR), RCC_AHB6RSTSETR_ETH1MACRST>;
using ETH2MAC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTSETR), RCC_AHB6RSTSETR_ETH2MACRST>;
using FDCAN_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTSETR), RCC_APB2RSTSETR_FDCANRST>;
using FMC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTSETR), RCC_AHB6RSTSETR_FMCRST>;
using GPIOA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTSETR), RCC_AHB4RSTSETR_GPIOARST>;
using GPIOB_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTSETR), RCC_AHB4RSTSETR_GPIOBRST>;
using GPIOC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTSETR), RCC_AHB4RSTSETR_GPIOCRST>;
using GPIOD_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTSETR), RCC_AHB4RSTSETR_GPIODRST>;
using GPIOE_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTSETR), RCC_AHB4RSTSETR_GPIOERST>;
using GPIOF_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTSETR), RCC_AHB4RSTSETR_GPIOFRST>;
using GPIOG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTSETR), RCC_AHB4RSTSETR_GPIOGRST>;
using GPIOH_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTSETR), RCC_AHB4RSTSETR_GPIOHRST>;
using GPIOI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTSETR), RCC_AHB4RSTSETR_GPIOIRST>;
using HASH1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB5RSTSETR), RCC_AHB5RSTSETR_HASH1RST>;
using I2C1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_I2C1RST>;
using I2C2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_I2C2RST>;
using I2C3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTSETR), RCC_APB6RSTSETR_I2C3RST>;
using I2C4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTSETR), RCC_APB6RSTSETR_I2C4RST>;
using I2C5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTSETR), RCC_APB6RSTSETR_I2C5RST>;
using LPTIM1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_LPTIM1RST>;
using LPTIM2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTSETR), RCC_APB3RSTSETR_LPTIM2RST>;
using LPTIM3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTSETR), RCC_APB3RSTSETR_LPTIM3RST>;
using LPTIM4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTSETR), RCC_APB3RSTSETR_LPTIM4RST>;
using LPTIM5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTSETR), RCC_APB3RSTSETR_LPTIM5RST>;
using LTDC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4RSTSETR), RCC_APB4RSTSETR_LTDCRST>;
using MCE_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTSETR), RCC_AHB6RSTSETR_MCERST>;
using MDMA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTSETR), RCC_AHB6RSTSETR_MDMARST>;
using PKA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB5RSTSETR), RCC_AHB5RSTSETR_PKARST>;
using PMBCTRL_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTSETR), RCC_APB3RSTSETR_PMBCTRLRST>;
using QSPI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTSETR), RCC_AHB6RSTSETR_QSPIRST>;
using RNG1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB5RSTSETR), RCC_AHB5RSTSETR_RNG1RST>;
using SAES_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB5RSTSETR), RCC_AHB5RSTSETR_SAESRST>;
using SAI1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTSETR), RCC_APB2RSTSETR_SAI1RST>;
using SAI2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTSETR), RCC_APB2RSTSETR_SAI2RST>;
using SDMMC1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTSETR), RCC_AHB6RSTSETR_SDMMC1RST>;
using SDMMC2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTSETR), RCC_AHB6RSTSETR_SDMMC2RST>;
using SPDIF_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_SPDIFRST>;
using SPI1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTSETR), RCC_APB2RSTSETR_SPI1RST>;
using SPI2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_SPI2RST>;
using SPI3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_SPI3RST>;
using SPI4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTSETR), RCC_APB6RSTSETR_SPI4RST>;
using SPI5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTSETR), RCC_APB6RSTSETR_SPI5RST>;
using STGEN_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB5RSTSETR), RCC_APB5RSTSETR_STGENRST>;
using SYSCFG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTSETR), RCC_APB3RSTSETR_SYSCFGRST>;
using TIM12_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTSETR), RCC_APB6RSTSETR_TIM12RST>;
using TIM13_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTSETR), RCC_APB6RSTSETR_TIM13RST>;
using TIM14_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTSETR), RCC_APB6RSTSETR_TIM14RST>;
using TIM15_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTSETR), RCC_APB6RSTSETR_TIM15RST>;
using TIM16_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTSETR), RCC_APB6RSTSETR_TIM16RST>;
using TIM17_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTSETR), RCC_APB6RSTSETR_TIM17RST>;
using TIM1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTSETR), RCC_APB2RSTSETR_TIM1RST>;
using TIM2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_TIM2RST>;
using TIM3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_TIM3RST>;
using TIM4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_TIM4RST>;
using TIM5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_TIM5RST>;
using TIM6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_TIM6RST>;
using TIM7_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_TIM7RST>;
using TIM8_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTSETR), RCC_APB2RSTSETR_TIM8RST>;
using UART4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_UART4RST>;
using UART5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_UART5RST>;
using UART7_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_UART7RST>;
using UART8_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_UART8RST>;
using USART1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTSETR), RCC_APB6RSTSETR_USART1RST>;
using USART2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTSETR), RCC_APB6RSTSETR_USART2RST>;
using USART3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_USART3RST>;
using USART6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTSETR), RCC_APB2RSTSETR_USART6RST>;
using USBH_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTSETR), RCC_AHB6RSTSETR_USBHRST>;
using USBO_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTSETR), RCC_AHB2RSTSETR_USBORST>;
using USBPHY_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4RSTSETR), RCC_APB4RSTSETR_USBPHYRST>;
using VREF_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTSETR), RCC_APB3RSTSETR_VREFRST>;
// clang-format on
} // namespace RCC_Reset_Set

namespace RCC_Reset_Release
{
// clang-format off

// Auto-generated:
// %s/\v#define RCC_(.{-}RSTCLRR)_([^ ]{-})RST[^_] /using \2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, \1), RCC_\1_\2RST>;\r\0/
using ADC1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTCLRR), RCC_AHB2RSTCLRR_ADC1RST>;
using ADC2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTCLRR), RCC_AHB2RSTCLRR_ADC2RST>;
using AXIMC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB5RSTCLRR), RCC_AHB5RSTCLRR_AXIMCRST>;
using CRC1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTCLRR), RCC_AHB6RSTCLRR_CRC1RST>;
using CRYP1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB5RSTCLRR), RCC_AHB5RSTCLRR_CRYP1RST>;
using DCMIPP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4RSTCLRR), RCC_APB4RSTCLRR_DCMIPPRST>;
using DDRPERFM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4RSTCLRR), RCC_APB4RSTCLRR_DDRPERFMRST>;
using DFSDM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTCLRR), RCC_APB2RSTCLRR_DFSDMRST>;
using DMA1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTCLRR), RCC_AHB2RSTCLRR_DMA1RST>;
using DMA2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTCLRR), RCC_AHB2RSTCLRR_DMA2RST>;
using DMA3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTCLRR), RCC_AHB2RSTCLRR_DMA3RST>;
using DMAMUX1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTCLRR), RCC_AHB2RSTCLRR_DMAMUX1RST>;
using DMAMUX2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTCLRR), RCC_AHB2RSTCLRR_DMAMUX2RST>;
using DTS_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTCLRR), RCC_APB3RSTCLRR_DTSRST>;
using ETH1MAC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTCLRR), RCC_AHB6RSTCLRR_ETH1MACRST>;
using ETH2MAC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTCLRR), RCC_AHB6RSTCLRR_ETH2MACRST>;
using FDCAN_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTCLRR), RCC_APB2RSTCLRR_FDCANRST>;
using FMC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTCLRR), RCC_AHB6RSTCLRR_FMCRST>;
using GPIOA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTCLRR), RCC_AHB4RSTCLRR_GPIOARST>;
using GPIOB_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTCLRR), RCC_AHB4RSTCLRR_GPIOBRST>;
using GPIOC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTCLRR), RCC_AHB4RSTCLRR_GPIOCRST>;
using GPIOD_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTCLRR), RCC_AHB4RSTCLRR_GPIODRST>;
using GPIOE_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTCLRR), RCC_AHB4RSTCLRR_GPIOERST>;
using GPIOF_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTCLRR), RCC_AHB4RSTCLRR_GPIOFRST>;
using GPIOG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTCLRR), RCC_AHB4RSTCLRR_GPIOGRST>;
using GPIOH_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTCLRR), RCC_AHB4RSTCLRR_GPIOHRST>;
using GPIOI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTCLRR), RCC_AHB4RSTCLRR_GPIOIRST>;
using HASH1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB5RSTCLRR), RCC_AHB5RSTCLRR_HASH1RST>;
using I2C1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTCLRR), RCC_APB1RSTCLRR_I2C1RST>;
using I2C2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTCLRR), RCC_APB1RSTCLRR_I2C2RST>;
using I2C3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTCLRR), RCC_APB6RSTCLRR_I2C3RST>;
using I2C4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTCLRR), RCC_APB6RSTCLRR_I2C4RST>;
using I2C5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTCLRR), RCC_APB6RSTCLRR_I2C5RST>;
using LPTIM1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTCLRR), RCC_APB1RSTCLRR_LPTIM1RST>;
using LPTIM2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTCLRR), RCC_APB3RSTCLRR_LPTIM2RST>;
using LPTIM3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTCLRR), RCC_APB3RSTCLRR_LPTIM3RST>;
using LPTIM4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTCLRR), RCC_APB3RSTCLRR_LPTIM4RST>;
using LPTIM5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTCLRR), RCC_APB3RSTCLRR_LPTIM5RST>;
using LTDC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4RSTCLRR), RCC_APB4RSTCLRR_LTDCRST>;
using MCE_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTCLRR), RCC_AHB6RSTCLRR_MCERST>;
using MDMA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTCLRR), RCC_AHB6RSTCLRR_MDMARST>;
using PKA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB5RSTCLRR), RCC_AHB5RSTCLRR_PKARST>;
using PMBCTRL_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTCLRR), RCC_APB3RSTCLRR_PMBCTRLRST>;
using QSPI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTCLRR), RCC_AHB6RSTCLRR_QSPIRST>;
using RNG1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB5RSTCLRR), RCC_AHB5RSTCLRR_RNG1RST>;
using SAES_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB5RSTCLRR), RCC_AHB5RSTCLRR_SAESRST>;
using SAI1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTCLRR), RCC_APB2RSTCLRR_SAI1RST>;
using SAI2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTCLRR), RCC_APB2RSTCLRR_SAI2RST>;
using SDMMC1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTCLRR), RCC_AHB6RSTCLRR_SDMMC1RST>;
using SDMMC2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTCLRR), RCC_AHB6RSTCLRR_SDMMC2RST>;
using SPDIF_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTCLRR), RCC_APB1RSTCLRR_SPDIFRST>;
using SPI1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTCLRR), RCC_APB2RSTCLRR_SPI1RST>;
using SPI2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTCLRR), RCC_APB1RSTCLRR_SPI2RST>;
using SPI3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTCLRR), RCC_APB1RSTCLRR_SPI3RST>;
using SPI4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTCLRR), RCC_APB6RSTCLRR_SPI4RST>;
using SPI5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTCLRR), RCC_APB6RSTCLRR_SPI5RST>;
using STGEN_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB5RSTCLRR), RCC_APB5RSTCLRR_STGENRST>;
using SYSCFG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTCLRR), RCC_APB3RSTCLRR_SYSCFGRST>;
using TIM12_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTCLRR), RCC_APB6RSTCLRR_TIM12RST>;
using TIM13_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTCLRR), RCC_APB6RSTCLRR_TIM13RST>;
using TIM14_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTCLRR), RCC_APB6RSTCLRR_TIM14RST>;
using TIM15_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTCLRR), RCC_APB6RSTCLRR_TIM15RST>;
using TIM16_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTCLRR), RCC_APB6RSTCLRR_TIM16RST>;
using TIM17_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTCLRR), RCC_APB6RSTCLRR_TIM17RST>;
using TIM1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTCLRR), RCC_APB2RSTCLRR_TIM1RST>;
using TIM2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTCLRR), RCC_APB1RSTCLRR_TIM2RST>;
using TIM3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTCLRR), RCC_APB1RSTCLRR_TIM3RST>;
using TIM4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTCLRR), RCC_APB1RSTCLRR_TIM4RST>;
using TIM5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTCLRR), RCC_APB1RSTCLRR_TIM5RST>;
using TIM6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTCLRR), RCC_APB1RSTCLRR_TIM6RST>;
using TIM7_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTCLRR), RCC_APB1RSTCLRR_TIM7RST>;
using TIM8_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTCLRR), RCC_APB2RSTCLRR_TIM8RST>;
using UART4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTCLRR), RCC_APB1RSTCLRR_UART4RST>;
using UART5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTCLRR), RCC_APB1RSTCLRR_UART5RST>;
using UART7_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTCLRR), RCC_APB1RSTCLRR_UART7RST>;
using UART8_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTCLRR), RCC_APB1RSTCLRR_UART8RST>;
using USART1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTCLRR), RCC_APB6RSTCLRR_USART1RST>;
using USART2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB6RSTCLRR), RCC_APB6RSTCLRR_USART2RST>;
using USART3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTCLRR), RCC_APB1RSTCLRR_USART3RST>;
using USART6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTCLRR), RCC_APB2RSTCLRR_USART6RST>;
using USBH_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTCLRR), RCC_AHB6RSTCLRR_USBHRST>;
using USBO_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTCLRR), RCC_AHB2RSTCLRR_USBORST>;
using USBPHY_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4RSTCLRR), RCC_APB4RSTCLRR_USBPHYRST>;
using VREF_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTCLRR), RCC_APB3RSTCLRR_VREFRST>;

// clang-format on
} // namespace RCC_Reset_Release

namespace RCC_Reset
{
namespace Set = RCC_Reset_Set;
namespace Clr = RCC_Reset_Release;
// clang-format off

// Auto-generated:
// ,$s/\vusing (.{-}_) \= Register\zsBits.*$/DualSetClear<Set::\1::BaseAddress, Set::\1::Mask, Clr::\1::BaseAddress, Clr::\1::Mask>;/

using ADC1_ = RegisterDualSetClear<Set::ADC1_::BaseAddress, Set::ADC1_::Mask, Clr::ADC1_::BaseAddress, Clr::ADC1_::Mask>;
using ADC2_ = RegisterDualSetClear<Set::ADC2_::BaseAddress, Set::ADC2_::Mask, Clr::ADC2_::BaseAddress, Clr::ADC2_::Mask>;
using AXIMC_ = RegisterDualSetClear<Set::AXIMC_::BaseAddress, Set::AXIMC_::Mask, Clr::AXIMC_::BaseAddress, Clr::AXIMC_::Mask>;
using CRC1_ = RegisterDualSetClear<Set::CRC1_::BaseAddress, Set::CRC1_::Mask, Clr::CRC1_::BaseAddress, Clr::CRC1_::Mask>;
using CRYP1_ = RegisterDualSetClear<Set::CRYP1_::BaseAddress, Set::CRYP1_::Mask, Clr::CRYP1_::BaseAddress, Clr::CRYP1_::Mask>;
using DCMIPP_ = RegisterDualSetClear<Set::DCMIPP_::BaseAddress, Set::DCMIPP_::Mask, Clr::DCMIPP_::BaseAddress, Clr::DCMIPP_::Mask>;
using DDRPERFM_ = RegisterDualSetClear<Set::DDRPERFM_::BaseAddress, Set::DDRPERFM_::Mask, Clr::DDRPERFM_::BaseAddress, Clr::DDRPERFM_::Mask>;
using DFSDM_ = RegisterDualSetClear<Set::DFSDM_::BaseAddress, Set::DFSDM_::Mask, Clr::DFSDM_::BaseAddress, Clr::DFSDM_::Mask>;
using DMA1_ = RegisterDualSetClear<Set::DMA1_::BaseAddress, Set::DMA1_::Mask, Clr::DMA1_::BaseAddress, Clr::DMA1_::Mask>;
using DMA2_ = RegisterDualSetClear<Set::DMA2_::BaseAddress, Set::DMA2_::Mask, Clr::DMA2_::BaseAddress, Clr::DMA2_::Mask>;
using DMA3_ = RegisterDualSetClear<Set::DMA3_::BaseAddress, Set::DMA3_::Mask, Clr::DMA3_::BaseAddress, Clr::DMA3_::Mask>;
using DMAMUX1_ = RegisterDualSetClear<Set::DMAMUX1_::BaseAddress, Set::DMAMUX1_::Mask, Clr::DMAMUX1_::BaseAddress, Clr::DMAMUX1_::Mask>;
using DMAMUX2_ = RegisterDualSetClear<Set::DMAMUX2_::BaseAddress, Set::DMAMUX2_::Mask, Clr::DMAMUX2_::BaseAddress, Clr::DMAMUX2_::Mask>;
using DTS_ = RegisterDualSetClear<Set::DTS_::BaseAddress, Set::DTS_::Mask, Clr::DTS_::BaseAddress, Clr::DTS_::Mask>;
using ETH1MAC_ = RegisterDualSetClear<Set::ETH1MAC_::BaseAddress, Set::ETH1MAC_::Mask, Clr::ETH1MAC_::BaseAddress, Clr::ETH1MAC_::Mask>;
using ETH2MAC_ = RegisterDualSetClear<Set::ETH2MAC_::BaseAddress, Set::ETH2MAC_::Mask, Clr::ETH2MAC_::BaseAddress, Clr::ETH2MAC_::Mask>;
using FDCAN_ = RegisterDualSetClear<Set::FDCAN_::BaseAddress, Set::FDCAN_::Mask, Clr::FDCAN_::BaseAddress, Clr::FDCAN_::Mask>;
using FMC_ = RegisterDualSetClear<Set::FMC_::BaseAddress, Set::FMC_::Mask, Clr::FMC_::BaseAddress, Clr::FMC_::Mask>;
using GPIOA_ = RegisterDualSetClear<Set::GPIOA_::BaseAddress, Set::GPIOA_::Mask, Clr::GPIOA_::BaseAddress, Clr::GPIOA_::Mask>;
using GPIOB_ = RegisterDualSetClear<Set::GPIOB_::BaseAddress, Set::GPIOB_::Mask, Clr::GPIOB_::BaseAddress, Clr::GPIOB_::Mask>;
using GPIOC_ = RegisterDualSetClear<Set::GPIOC_::BaseAddress, Set::GPIOC_::Mask, Clr::GPIOC_::BaseAddress, Clr::GPIOC_::Mask>;
using GPIOD_ = RegisterDualSetClear<Set::GPIOD_::BaseAddress, Set::GPIOD_::Mask, Clr::GPIOD_::BaseAddress, Clr::GPIOD_::Mask>;
using GPIOE_ = RegisterDualSetClear<Set::GPIOE_::BaseAddress, Set::GPIOE_::Mask, Clr::GPIOE_::BaseAddress, Clr::GPIOE_::Mask>;
using GPIOF_ = RegisterDualSetClear<Set::GPIOF_::BaseAddress, Set::GPIOF_::Mask, Clr::GPIOF_::BaseAddress, Clr::GPIOF_::Mask>;
using GPIOG_ = RegisterDualSetClear<Set::GPIOG_::BaseAddress, Set::GPIOG_::Mask, Clr::GPIOG_::BaseAddress, Clr::GPIOG_::Mask>;
using GPIOH_ = RegisterDualSetClear<Set::GPIOH_::BaseAddress, Set::GPIOH_::Mask, Clr::GPIOH_::BaseAddress, Clr::GPIOH_::Mask>;
using GPIOI_ = RegisterDualSetClear<Set::GPIOI_::BaseAddress, Set::GPIOI_::Mask, Clr::GPIOI_::BaseAddress, Clr::GPIOI_::Mask>;
using HASH1_ = RegisterDualSetClear<Set::HASH1_::BaseAddress, Set::HASH1_::Mask, Clr::HASH1_::BaseAddress, Clr::HASH1_::Mask>;
using I2C1_ = RegisterDualSetClear<Set::I2C1_::BaseAddress, Set::I2C1_::Mask, Clr::I2C1_::BaseAddress, Clr::I2C1_::Mask>;
using I2C2_ = RegisterDualSetClear<Set::I2C2_::BaseAddress, Set::I2C2_::Mask, Clr::I2C2_::BaseAddress, Clr::I2C2_::Mask>;
using I2C3_ = RegisterDualSetClear<Set::I2C3_::BaseAddress, Set::I2C3_::Mask, Clr::I2C3_::BaseAddress, Clr::I2C3_::Mask>;
using I2C4_ = RegisterDualSetClear<Set::I2C4_::BaseAddress, Set::I2C4_::Mask, Clr::I2C4_::BaseAddress, Clr::I2C4_::Mask>;
using I2C5_ = RegisterDualSetClear<Set::I2C5_::BaseAddress, Set::I2C5_::Mask, Clr::I2C5_::BaseAddress, Clr::I2C5_::Mask>;
using LPTIM1_ = RegisterDualSetClear<Set::LPTIM1_::BaseAddress, Set::LPTIM1_::Mask, Clr::LPTIM1_::BaseAddress, Clr::LPTIM1_::Mask>;
using LPTIM2_ = RegisterDualSetClear<Set::LPTIM2_::BaseAddress, Set::LPTIM2_::Mask, Clr::LPTIM2_::BaseAddress, Clr::LPTIM2_::Mask>;
using LPTIM3_ = RegisterDualSetClear<Set::LPTIM3_::BaseAddress, Set::LPTIM3_::Mask, Clr::LPTIM3_::BaseAddress, Clr::LPTIM3_::Mask>;
using LPTIM4_ = RegisterDualSetClear<Set::LPTIM4_::BaseAddress, Set::LPTIM4_::Mask, Clr::LPTIM4_::BaseAddress, Clr::LPTIM4_::Mask>;
using LPTIM5_ = RegisterDualSetClear<Set::LPTIM5_::BaseAddress, Set::LPTIM5_::Mask, Clr::LPTIM5_::BaseAddress, Clr::LPTIM5_::Mask>;
using LTDC_ = RegisterDualSetClear<Set::LTDC_::BaseAddress, Set::LTDC_::Mask, Clr::LTDC_::BaseAddress, Clr::LTDC_::Mask>;
using MCE_ = RegisterDualSetClear<Set::MCE_::BaseAddress, Set::MCE_::Mask, Clr::MCE_::BaseAddress, Clr::MCE_::Mask>;
using MDMA_ = RegisterDualSetClear<Set::MDMA_::BaseAddress, Set::MDMA_::Mask, Clr::MDMA_::BaseAddress, Clr::MDMA_::Mask>;
using PKA_ = RegisterDualSetClear<Set::PKA_::BaseAddress, Set::PKA_::Mask, Clr::PKA_::BaseAddress, Clr::PKA_::Mask>;
using PMBCTRL_ = RegisterDualSetClear<Set::PMBCTRL_::BaseAddress, Set::PMBCTRL_::Mask, Clr::PMBCTRL_::BaseAddress, Clr::PMBCTRL_::Mask>;
using QSPI_ = RegisterDualSetClear<Set::QSPI_::BaseAddress, Set::QSPI_::Mask, Clr::QSPI_::BaseAddress, Clr::QSPI_::Mask>;
using RNG1_ = RegisterDualSetClear<Set::RNG1_::BaseAddress, Set::RNG1_::Mask, Clr::RNG1_::BaseAddress, Clr::RNG1_::Mask>;
using SAES_ = RegisterDualSetClear<Set::SAES_::BaseAddress, Set::SAES_::Mask, Clr::SAES_::BaseAddress, Clr::SAES_::Mask>;
using SAI1_ = RegisterDualSetClear<Set::SAI1_::BaseAddress, Set::SAI1_::Mask, Clr::SAI1_::BaseAddress, Clr::SAI1_::Mask>;
using SAI2_ = RegisterDualSetClear<Set::SAI2_::BaseAddress, Set::SAI2_::Mask, Clr::SAI2_::BaseAddress, Clr::SAI2_::Mask>;
using SDMMC1_ = RegisterDualSetClear<Set::SDMMC1_::BaseAddress, Set::SDMMC1_::Mask, Clr::SDMMC1_::BaseAddress, Clr::SDMMC1_::Mask>;
using SDMMC2_ = RegisterDualSetClear<Set::SDMMC2_::BaseAddress, Set::SDMMC2_::Mask, Clr::SDMMC2_::BaseAddress, Clr::SDMMC2_::Mask>;
using SPDIF_ = RegisterDualSetClear<Set::SPDIF_::BaseAddress, Set::SPDIF_::Mask, Clr::SPDIF_::BaseAddress, Clr::SPDIF_::Mask>;
using SPI1_ = RegisterDualSetClear<Set::SPI1_::BaseAddress, Set::SPI1_::Mask, Clr::SPI1_::BaseAddress, Clr::SPI1_::Mask>;
using SPI2_ = RegisterDualSetClear<Set::SPI2_::BaseAddress, Set::SPI2_::Mask, Clr::SPI2_::BaseAddress, Clr::SPI2_::Mask>;
using SPI3_ = RegisterDualSetClear<Set::SPI3_::BaseAddress, Set::SPI3_::Mask, Clr::SPI3_::BaseAddress, Clr::SPI3_::Mask>;
using SPI4_ = RegisterDualSetClear<Set::SPI4_::BaseAddress, Set::SPI4_::Mask, Clr::SPI4_::BaseAddress, Clr::SPI4_::Mask>;
using SPI5_ = RegisterDualSetClear<Set::SPI5_::BaseAddress, Set::SPI5_::Mask, Clr::SPI5_::BaseAddress, Clr::SPI5_::Mask>;
using STGEN_ = RegisterDualSetClear<Set::STGEN_::BaseAddress, Set::STGEN_::Mask, Clr::STGEN_::BaseAddress, Clr::STGEN_::Mask>;
using SYSCFG_ = RegisterDualSetClear<Set::SYSCFG_::BaseAddress, Set::SYSCFG_::Mask, Clr::SYSCFG_::BaseAddress, Clr::SYSCFG_::Mask>;
using TIM12_ = RegisterDualSetClear<Set::TIM12_::BaseAddress, Set::TIM12_::Mask, Clr::TIM12_::BaseAddress, Clr::TIM12_::Mask>;
using TIM13_ = RegisterDualSetClear<Set::TIM13_::BaseAddress, Set::TIM13_::Mask, Clr::TIM13_::BaseAddress, Clr::TIM13_::Mask>;
using TIM14_ = RegisterDualSetClear<Set::TIM14_::BaseAddress, Set::TIM14_::Mask, Clr::TIM14_::BaseAddress, Clr::TIM14_::Mask>;
using TIM15_ = RegisterDualSetClear<Set::TIM15_::BaseAddress, Set::TIM15_::Mask, Clr::TIM15_::BaseAddress, Clr::TIM15_::Mask>;
using TIM16_ = RegisterDualSetClear<Set::TIM16_::BaseAddress, Set::TIM16_::Mask, Clr::TIM16_::BaseAddress, Clr::TIM16_::Mask>;
using TIM17_ = RegisterDualSetClear<Set::TIM17_::BaseAddress, Set::TIM17_::Mask, Clr::TIM17_::BaseAddress, Clr::TIM17_::Mask>;
using TIM1_ = RegisterDualSetClear<Set::TIM1_::BaseAddress, Set::TIM1_::Mask, Clr::TIM1_::BaseAddress, Clr::TIM1_::Mask>;
using TIM2_ = RegisterDualSetClear<Set::TIM2_::BaseAddress, Set::TIM2_::Mask, Clr::TIM2_::BaseAddress, Clr::TIM2_::Mask>;
using TIM3_ = RegisterDualSetClear<Set::TIM3_::BaseAddress, Set::TIM3_::Mask, Clr::TIM3_::BaseAddress, Clr::TIM3_::Mask>;
using TIM4_ = RegisterDualSetClear<Set::TIM4_::BaseAddress, Set::TIM4_::Mask, Clr::TIM4_::BaseAddress, Clr::TIM4_::Mask>;
using TIM5_ = RegisterDualSetClear<Set::TIM5_::BaseAddress, Set::TIM5_::Mask, Clr::TIM5_::BaseAddress, Clr::TIM5_::Mask>;
using TIM6_ = RegisterDualSetClear<Set::TIM6_::BaseAddress, Set::TIM6_::Mask, Clr::TIM6_::BaseAddress, Clr::TIM6_::Mask>;
using TIM7_ = RegisterDualSetClear<Set::TIM7_::BaseAddress, Set::TIM7_::Mask, Clr::TIM7_::BaseAddress, Clr::TIM7_::Mask>;
using TIM8_ = RegisterDualSetClear<Set::TIM8_::BaseAddress, Set::TIM8_::Mask, Clr::TIM8_::BaseAddress, Clr::TIM8_::Mask>;
using UART4_ = RegisterDualSetClear<Set::UART4_::BaseAddress, Set::UART4_::Mask, Clr::UART4_::BaseAddress, Clr::UART4_::Mask>;
using UART5_ = RegisterDualSetClear<Set::UART5_::BaseAddress, Set::UART5_::Mask, Clr::UART5_::BaseAddress, Clr::UART5_::Mask>;
using UART7_ = RegisterDualSetClear<Set::UART7_::BaseAddress, Set::UART7_::Mask, Clr::UART7_::BaseAddress, Clr::UART7_::Mask>;
using UART8_ = RegisterDualSetClear<Set::UART8_::BaseAddress, Set::UART8_::Mask, Clr::UART8_::BaseAddress, Clr::UART8_::Mask>;
using USART1_ = RegisterDualSetClear<Set::USART1_::BaseAddress, Set::USART1_::Mask, Clr::USART1_::BaseAddress, Clr::USART1_::Mask>;
using USART2_ = RegisterDualSetClear<Set::USART2_::BaseAddress, Set::USART2_::Mask, Clr::USART2_::BaseAddress, Clr::USART2_::Mask>;
using USART3_ = RegisterDualSetClear<Set::USART3_::BaseAddress, Set::USART3_::Mask, Clr::USART3_::BaseAddress, Clr::USART3_::Mask>;
using USART6_ = RegisterDualSetClear<Set::USART6_::BaseAddress, Set::USART6_::Mask, Clr::USART6_::BaseAddress, Clr::USART6_::Mask>;
using USBH_ = RegisterDualSetClear<Set::USBH_::BaseAddress, Set::USBH_::Mask, Clr::USBH_::BaseAddress, Clr::USBH_::Mask>;
using USBO_ = RegisterDualSetClear<Set::USBO_::BaseAddress, Set::USBO_::Mask, Clr::USBO_::BaseAddress, Clr::USBO_::Mask>;
using USBPHY_ = RegisterDualSetClear<Set::USBPHY_::BaseAddress, Set::USBPHY_::Mask, Clr::USBPHY_::BaseAddress, Clr::USBPHY_::Mask>;
using VREF_ = RegisterDualSetClear<Set::VREF_::BaseAddress, Set::VREF_::Mask, Clr::VREF_::BaseAddress, Clr::VREF_::Mask>;

// Aliases:
using DMAMUX_ = DMAMUX1_;

// Blanks:
using ADC3_ = NonexistantRegister;
using I2C6_ = NonexistantRegister;
using BDMA_ = NonexistantRegister;
using TIM9_ = NonexistantRegister;
using TIM10_ = NonexistantRegister;
using TIM11_ = NonexistantRegister;
using SAI3_ = NonexistantRegister;
using SAI4_ = NonexistantRegister;
using SPI6_ = NonexistantRegister;
}

} // namespace mdrivlib
