#pragma once
#include "register_access.hh"
#include "stm32xx.h"

namespace mdrivlib
{
namespace RCC_Enable
{
using RegisterDataT = uint32_t;

using ADC1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_ADC1EN>;
using ADC2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_ADC2EN>;
using ADC3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_ADC3EN>;
using BKPSRAM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_BKPSRAMEN>;
using CAN1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_CAN1EN>;
using CAN2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_CAN2EN>;
using CEC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_CECEN>;
using CRC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_CRCEN>;
using DAC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_DACEN>;
using DCMI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2ENR), RCC_AHB2ENR_DCMIEN>;
using DMA1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_DMA1EN>;
using DMA2D_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_DMA2DEN>;
using DMA2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_DMA2EN>;
using DTCMRAM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_DTCMRAMEN>;
using ETHMACPTP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_ETHMACPTPEN>;
using ETHMACRX_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_ETHMACRXEN>;
using ETHMACTX_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_ETHMACTXEN>;
using ETHMAC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_ETHMACEN>;
using FMC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB3ENR), RCC_AHB3ENR_FMCEN>;
using GPIOA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_GPIOAEN>;
using GPIOB_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_GPIOBEN>;
using GPIOC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_GPIOCEN>;
using GPIOD_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_GPIODEN>;
using GPIOE_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_GPIOEEN>;
using GPIOF_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_GPIOFEN>;
using GPIOG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_GPIOGEN>;
using GPIOH_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_GPIOHEN>;
using GPIOI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_GPIOIEN>;
using GPIOJ_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_GPIOJEN>;
using GPIOK_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_GPIOKEN>;
using I2C1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_I2C1EN>;
using I2C2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_I2C2EN>;
using I2C3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_I2C3EN>;
using I2C4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_I2C4EN>;
using LPTIM1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_LPTIM1EN>;
using LTDC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_LTDCEN>;
using OTGFS_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2ENR), RCC_AHB2ENR_OTGFSEN>;
using OTGHSULPI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_OTGHSULPIEN>;
using OTGHS_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_OTGHSEN>;
using PWR_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_PWREN>;
using QSPI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB3ENR), RCC_AHB3ENR_QSPIEN>;
using RNG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2ENR), RCC_AHB2ENR_RNGEN>;
using SAI1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SAI1EN>;
using SAI2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SAI2EN>;
using SDMMC1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SDMMC1EN>;
using SPDIFRX_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_SPDIFRXEN>;
using SPI1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SPI1EN>;
using SPI2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_SPI2EN>;
using SPI3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_SPI3EN>;
using SPI4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SPI4EN>;
using SPI5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SPI5EN>;
using SPI6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SPI6EN>;
using SYSCFG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SYSCFGEN>;
using TIM10_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM10EN>;
using TIM11_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM11EN>;
using TIM12_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_TIM12EN>;
using TIM13_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_TIM13EN>;
using TIM14_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_TIM14EN>;
using TIM1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM1EN>;
using TIM2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_TIM2EN>;
using TIM3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_TIM3EN>;
using TIM4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_TIM4EN>;
using TIM5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_TIM5EN>;
using TIM6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_TIM6EN>;
using TIM7_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_TIM7EN>;
using TIM8_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM8EN>;
using TIM9_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM9EN>;
using UART4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_UART4EN>;
using UART5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_UART5EN>;
using UART7_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_UART7EN>;
using UART8_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_UART8EN>;
using USART1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_USART1EN>;
using USART2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_USART2EN>;
using USART3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1ENR), RCC_APB1ENR_USART3EN>;
using USART6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_USART6EN>;
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
		else if (periph == GPIOF)
			GPIOF_::set();
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
		else if (periph == GPIOF)
			GPIOF_::clear();
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
		else if (periph == GPIOF)
			return GPIOF_::read();
		return false;
	}
};

using BDMA_ = NonexistantRegister;
using DMAMUX_ = NonexistantRegister;
using I2C5_ = NonexistantRegister;
using I2C6_ = NonexistantRegister;

} // namespace RCC_Enable
} // namespace mdrivlib
