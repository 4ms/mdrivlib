#pragma once
#include "register_access.hh"
#include "stm32xx.h"

namespace mdrivlib
{
namespace RCC_Enable
{
using RegisterDataT = uint32_t;

// clang-format off

// Autogenerated by:
//:s/\v#define RCC_(.{-}ENR)_([^ ]{-})EN[^_] /using \2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, \1), RCC_\1_\2EN>;\r\0

using ADC1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_ADC1EN>;
using AES_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2ENR), RCC_AHB2ENR_AESEN>;
using CAN1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_CAN1EN>;
using CAN2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_CAN2EN>;
using CAN3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_CAN3EN>;
using CRC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_CRCEN>;
using DAC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_DACEN>;
using DFSDM1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_DFSDM1EN>;
using DFSDM2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_DFSDM2EN>;
using DMA1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_DMA1EN>;
using DMA2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_DMA2EN>;
using EXTIT_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_EXTITEN>;
using FMPI2C1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_FMPI2C1EN>;
using FSMC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB3ENR), RCC_AHB3ENR_FSMCEN>;
using GPIOA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_GPIOAEN>;
using GPIOB_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_GPIOBEN>;
using GPIOC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_GPIOCEN>;
using GPIOD_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_GPIODEN>;
using GPIOE_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_GPIOEEN>;
using GPIOF_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_GPIOFEN>;
using GPIOG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_GPIOGEN>;
using GPIOH_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_GPIOHEN>;
using I2C1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_I2C1EN>;
using I2C2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_I2C2EN>;
using I2C3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_I2C3EN>;
using LPTIM1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_LPTIM1EN>;
using OTGFS_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2ENR), RCC_AHB2ENR_OTGFSEN>;
using PWR_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_PWREN>;
using QSPI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB3ENR), RCC_AHB3ENR_QSPIEN>;
using RNG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2ENR), RCC_AHB2ENR_RNGEN>;
using RTCAPB_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_RTCAPBEN>;
using SAI1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SAI1EN>;
using SDIO_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SDIOEN>;
using SPI1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SPI1EN>;
using SPI2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_SPI2EN>;
using SPI3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_SPI3EN>;
using SPI4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SPI4EN>;
using SPI5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SPI5EN>;
using SYSCFG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SYSCFGEN>;
using TIM1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM1EN>;
using TIM2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_TIM2EN>;
using TIM3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_TIM3EN>;
using TIM4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_TIM4EN>;
using TIM5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_TIM5EN>;
using TIM6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_TIM6EN>;
using TIM7_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_TIM7EN>;
using TIM8_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM8EN>;
using TIM9_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM9EN>;
using TIM10_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM10EN>;
using TIM11_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM11EN>;
using TIM12_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_TIM12EN>;
using TIM13_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_TIM13EN>;
using TIM14_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_TIM14EN>;
using USART1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_USART1EN>;
using USART2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_USART2EN>;
using USART3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_USART3EN>;
using UART4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_UART4EN>;
using UART5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_UART5EN>;
using USART6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_USART6EN>;
using UART7_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_UART7EN>;
using UART8_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_UART8EN>;
using UART9_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_UART9EN>;
using UART10_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_UART10EN>;
using WWDG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_WWDGEN>;

struct GPIO {
	static void enable(GPIO_TypeDef *periph) {
		if (periph == GPIOA)
			GPIOA_::set();
		else if (periph == GPIOB)
			GPIOB_::set();
		else if (periph == GPIOC)
			GPIOC_::set();
		else if (periph == GPIOD)
			GPIOD_::set();
		else if (periph == GPIOE)
			GPIOE_::set();
		else if (periph == GPIOF)
			GPIOF_::set();
		else if (periph == GPIOG)
			GPIOG_::set();
		else if (periph == GPIOH)
			GPIOH_::set();
		[[maybe_unused]] bool delay_after_enabling = is_enabled(periph);
	}
	static void disable(GPIO_TypeDef *periph) {
		if (periph == GPIOA)
			GPIOA_::clear();
		else if (periph == GPIOB)
			GPIOB_::clear();
		else if (periph == GPIOC)
			GPIOC_::clear();
		else if (periph == GPIOD)
			GPIOD_::clear();
		else if (periph == GPIOE)
			GPIOE_::clear();
		else if (periph == GPIOF)
			GPIOF_::clear();
		else if (periph == GPIOG)
			GPIOG_::clear();
		else if (periph == GPIOH)
			GPIOH_::clear();
		[[maybe_unused]] bool delay_after_disabling = is_enabled(periph);
	}
	static bool is_enabled(GPIO_TypeDef *periph) {
		if (periph == GPIOA)
			return GPIOA_::read();
		else if (periph == GPIOB)
			return GPIOB_::read();
		else if (periph == GPIOC)
			return GPIOC_::read();
		else if (periph == GPIOD)
			return GPIOD_::read();
		else if (periph == GPIOE)
			return GPIOE_::read();
		else if (periph == GPIOF)
			return GPIOF_::read();
		else if (periph == GPIOG)
			return GPIOG_::read();
		else if (periph == GPIOH)
			return GPIOH_::read();
		return false;
	}
};

using GPIOI_ = NonexistantRegister;
using GPIOJ_ = NonexistantRegister;
using BDMA_ = NonexistantRegister;
using DMAMUX_ = NonexistantRegister;
using I2C4_ = NonexistantRegister;
using I2C5_ = NonexistantRegister;
using I2C6_ = NonexistantRegister;
using SAI2_ = NonexistantRegister;
using SAI3_ = NonexistantRegister;
using SAI4_ = NonexistantRegister;
using TIM15_ = NonexistantRegister;
using TIM16_ = NonexistantRegister;
using TIM17_ = NonexistantRegister;

} // namespace RCC_Enable

namespace RCC_Reset {
using APB1 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), 0xFFFFFFFF>;
using APB2 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), 0xFFFFFFFF>;
using AHB1 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR), 0xFFFFFFFF>;
using AHB2 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTR), 0xFFFFFFFF>;
using AHB3 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB3RSTR), 0xFFFFFFFF>;

//:s/\v#define RCC_(.{-}RSTR)_([^ ]{-})RST[^_] /using \2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, \1), RCC_\1_\2RST>;\r\0/
using ADC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_ADCRST>;
using AES_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTR), RCC_AHB2RSTR_AESRST>;
using CAN1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_CAN1RST>;
using CAN2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_CAN2RST>;
using CAN3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_CAN3RST>;
using CRC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR), RCC_AHB1RSTR_CRCRST>;
using DAC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_DACRST>;
using DFSDM1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_DFSDM1RST>;
using DFSDM2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_DFSDM2RST>;
using DMA1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR), RCC_AHB1RSTR_DMA1RST>;
using DMA2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR), RCC_AHB1RSTR_DMA2RST>;
using FMPI2C1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_FMPI2C1RST>;
using FSMC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB3RSTR), RCC_AHB3RSTR_FSMCRST>;
using GPIOA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR), RCC_AHB1RSTR_GPIOARST>;
using GPIOB_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR), RCC_AHB1RSTR_GPIOBRST>;
using GPIOC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR), RCC_AHB1RSTR_GPIOCRST>;
using GPIOD_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR), RCC_AHB1RSTR_GPIODRST>;
using GPIOE_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR), RCC_AHB1RSTR_GPIOERST>;
using GPIOF_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR), RCC_AHB1RSTR_GPIOFRST>;
using GPIOG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR), RCC_AHB1RSTR_GPIOGRST>;
using GPIOH_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR), RCC_AHB1RSTR_GPIOHRST>;
using I2C1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_I2C1RST>;
using I2C2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_I2C2RST>;
using I2C3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_I2C3RST>;
using LPTIM1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_LPTIM1RST>;
using OTGFS_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTR), RCC_AHB2RSTR_OTGFSRST>;
using PWR_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_PWRRST>;
using QSPI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB3RSTR), RCC_AHB3RSTR_QSPIRST>;
using RNG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTR), RCC_AHB2RSTR_RNGRST>;
using SAI1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_SAI1RST>;
using SDIO_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_SDIORST>;
using SPI1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_SPI1RST>;
using SPI2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_SPI2RST>;
using SPI3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_SPI3RST>;
using SPI4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_SPI4RST>;
using SPI5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_SPI5RST>;
using SYSCFG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_SYSCFGRST>;
using TIM10_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_TIM10RST>;
using TIM11_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_TIM11RST>;
using TIM12_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_TIM12RST>;
using TIM13_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_TIM13RST>;
using TIM14_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_TIM14RST>;
using TIM1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_TIM1RST>;
using TIM2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_TIM2RST>;
using TIM3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_TIM3RST>;
using TIM4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_TIM4RST>;
using TIM5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_TIM5RST>;
using TIM6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_TIM6RST>;
using TIM7_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_TIM7RST>;
using TIM8_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_TIM8RST>;
using TIM9_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_TIM9RST>;
using UART10_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_UART10RST>;
using UART4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_UART4RST>;
using UART5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_UART5RST>;
using UART7_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_UART7RST>;
using UART8_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_UART8RST>;
using UART9_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_UART9RST>;
using USART1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_USART1RST>;
using USART2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_USART2RST>;
using USART3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_USART3RST>;
using USART6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_USART6RST>;
using WWDG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR), RCC_APB1RSTR_WWDGRST>;

using GPIOI_ = NonexistantRegister;
using GPIOJ_ = NonexistantRegister;
using BDMA_ = NonexistantRegister;
using DMAMUX_ = NonexistantRegister;
using I2C4_ = NonexistantRegister;
using I2C5_ = NonexistantRegister;
using I2C6_ = NonexistantRegister;
using SAI2_ = NonexistantRegister;
using SAI3_ = NonexistantRegister;
using SAI4_ = NonexistantRegister;
using TIM15_ = NonexistantRegister;
using TIM16_ = NonexistantRegister;
using TIM17_ = NonexistantRegister;

} // namespace RCC_Reset

//clang-format on
} // namespace mdrivlib
