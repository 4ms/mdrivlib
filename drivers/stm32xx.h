#pragma once
#ifdef STM32F7
#include "stm32f7xx.h"
#else
#ifdef STM32H7
#include "stm32h7xx.h"
#else
#ifdef STM32F4
#include "stm32f4xx.h"
#else
#ifdef STM32MP1
#ifdef CORE_CM4
#include "stm32mp1xx.h"
#else
#error "STM32MP1 is defined, but not CORE_CM4. This library does not support Cortex-A cores"
#endif
#else
#error "Please #define STM32F4, STM32F7, STM32H7, or STM32MP1 in a header or Makefile"
#endif
#endif
#endif
#endif

#ifndef ADC1
#define ADC1 nullptr
#endif
#ifndef ADC2
#define ADC2 nullptr
#endif
#ifndef ADC3
#define ADC3 nullptr
#endif

#ifndef DMA1
#define DMA1 nullptr
#endif
#ifndef DMA2
#define DMA2 nullptr
#endif

#ifndef I2C1
#define I2C1 nullptr
#endif
#ifndef I2C2
#define I2C2 nullptr
#endif
#ifndef I2C3
#define I2C3 nullptr
#endif

#ifndef SAI1
#define SAI1 nullptr
#endif
#ifndef SAI2
#define SAI2 nullptr
#endif
#ifndef SAI3
#define SAI3 nullptr
#endif
#ifndef SAI4
#define SAI4 nullptr
#endif

#ifndef TIM1
#define TIM1 nullptr
#endif
#ifndef TIM2
#define TIM2 nullptr
#endif
#ifndef TIM3
#define TIM3 nullptr
#endif
#ifndef TIM4
#define TIM4 nullptr
#endif
#ifndef TIM5
#define TIM5 nullptr
#endif
#ifndef TIM6
#define TIM6 nullptr
#endif
#ifndef TIM7
#define TIM7 nullptr
#endif
#ifndef TIM8
#define TIM8 nullptr
#endif
#ifndef TIM9
#define TIM9 nullptr
#endif
#ifndef TIM10
#define TIM10 nullptr
#endif
#ifndef TIM11
#define TIM11 nullptr
#endif
#ifndef TIM12
#define TIM12 nullptr
#endif
#ifndef TIM13
#define TIM13 nullptr
#endif
#ifndef TIM14
#define TIM14 nullptr
#endif
#ifndef TIM15
#define TIM15 nullptr
#endif
#ifndef TIM16
#define TIM16 nullptr
#endif
#ifndef TIM17
#define TIM17 nullptr
#endif

#ifndef SPI1
#define SPI1 nullptr
#endif
#ifndef SPI2
#define SPI2 nullptr
#endif
#ifndef SPI3
#define SPI3 nullptr
#endif
#ifndef SPI4
#define SPI4 nullptr
#endif
#ifndef SPI5
#define SPI5 nullptr
#endif
#ifndef SPI6
#define SPI6 nullptr
#endif

#ifndef SYSCFG_PMCR_I2C1_FMP
#define SYSCFG_PMCR_I2C1_FMP -1
#endif
#ifndef SYSCFG_PMCR_I2C2_FMP
#define SYSCFG_PMCR_I2C2_FMP -1
#endif
#ifndef SYSCFG_PMCR_I2C3_FMP
#define SYSCFG_PMCR_I2C3_FMP -1
#endif
