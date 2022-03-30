#include "drivers/register_access.hh"
#include "drivers/stm32xx.h"

namespace mdrivlib
{
namespace RCC_Enable
{
using ADC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_ADCEN>;
using CRC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHBENR), RCC_AHBENR_CRCEN>;
using DBGMCU_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_DBGMCUEN>;
using DMA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHBENR), RCC_AHBENR_DMAEN>;
using FLITF_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHBENR), RCC_AHBENR_FLITFEN>;
using GPIOA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHBENR), RCC_AHBENR_GPIOAEN>;
using GPIOB_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHBENR), RCC_AHBENR_GPIOBEN>;
using GPIOC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHBENR), RCC_AHBENR_GPIOCEN>;
using GPIOD_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHBENR), RCC_AHBENR_GPIODEN>;
using GPIOF_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHBENR), RCC_AHBENR_GPIOFEN>;
using I2C1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_I2C1EN>;
using PWR_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_PWREN>;
using SPI1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SPI1EN>;
using SRAM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHBENR), RCC_AHBENR_SRAMEN>;
using SYSCFGCOMP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SYSCFGCOMPEN>;
using TIM14_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_TIM14EN>;
using TIM16_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM16EN>;
using TIM17_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM17EN>;
using TIM1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM1EN>;
using TIM3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_TIM3EN>;
using USART1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_USART1EN>;
using WWDG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_WWDGEN>;
/*
s/\v#define RCC_(.{-}ENR)_([^ ]{-})EN[^_] /using \2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, \1), RCC_\1_\2EN>;\r\0
*/
} // namespace RCC_Enable

namespace RCC_Reset
{
using ADC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_ADCRST>;
using DBGMCU_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_DBGMCURST>;
using GPIOA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHBRSTR), RCC_AHBRSTR_GPIOARST>;
using GPIOB_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHBRSTR), RCC_AHBRSTR_GPIOBRST>;
using GPIOC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHBRSTR), RCC_AHBRSTR_GPIOCRST>;
using GPIOD_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHBRSTR), RCC_AHBRSTR_GPIODRST>;
using GPIOF_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHBRSTR), RCC_AHBRSTR_GPIOFRST>;
using I2C1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_I2C1RST>;
using PWR_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_PWRRST>;
using SPI1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_SPI1RST>;
using SYSCFG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_SYSCFGRST>;
using TIM14_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_TIM14RST>;
using TIM16_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_TIM16RST>;
using TIM17_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_TIM17RST>;
using TIM1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_TIM1RST>;
using TIM3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_TIM3RST>;
using USART1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_USART1RST>;
using WWDG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_WWDGRST>;
/*
s/\v#define RCC_(.{-}ENR)_([^ ]{-})EN[^_] /using \2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, \1), RCC_\1_\2EN>;\r\0
*/
} // namespace RCC_Reset

namespace RCC_Clocks
{
//
}
} // namespace mdrivlib
