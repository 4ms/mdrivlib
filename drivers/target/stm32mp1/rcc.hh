#pragma once
#include "register_access.hh"
#include "stm32xx.h"

namespace mdrivlib
{
namespace stm32mp1
{
namespace core_a7
{
// Todo: if both cores share same data, then put it in _rcc_common.hh and #include it for both
namespace RCC_Control
{
using RegisterDataT = uint32_t;

using GPIO_A = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4ENSETR), RCC_MC_AHB4ENSETR_GPIOAEN>;
using GPIO_B = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4ENSETR), RCC_MC_AHB4ENSETR_GPIOBEN>;
using GPIO_C = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4ENSETR), RCC_MC_AHB4ENSETR_GPIOCEN>;
using GPIO_D = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4ENSETR), RCC_MC_AHB4ENSETR_GPIODEN>;
using GPIO_E = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4ENSETR), RCC_MC_AHB4ENSETR_GPIOEEN>;
using GPIO_F = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4ENSETR), RCC_MC_AHB4ENSETR_GPIOFEN>;
using GPIO_G = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4ENSETR), RCC_MC_AHB4ENSETR_GPIOGEN>;
using GPIO_H = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4ENSETR), RCC_MC_AHB4ENSETR_GPIOHEN>;
using GPIO_I = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4ENSETR), RCC_MC_AHB4ENSETR_GPIOIEN>;
using GPIO_J = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4ENSETR), RCC_MC_AHB4ENSETR_GPIOJEN>;
using GPIO_Z = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB5ENSETR), RCC_MC_AHB5ENSETR_GPIOZEN>;

// special-case: GPIO port base address can be used to calc bit-offset of RCC enable bit
struct GPIO {
	static inline volatile RegisterDataT *const _reg = &(RCC->MC_AHB4ENSETR);

	static uint32_t get_gpio_bit(RegisterDataT periph) {
		return 1 << ((periph & 0x00003C00) >> 10);
	}
	static void enable(GPIO_TypeDef *periph) {
		if (periph == GPIOZ)
			GPIO_Z::set();
		else
			*_reg = *_reg | get_gpio_bit(reinterpret_cast<RegisterDataT>(periph));
		[[maybe_unused]] bool delay_after_enabling = is_enabled(periph);
	}
	static void disable(GPIO_TypeDef *periph) {
		if (periph == GPIOZ)
			GPIO_Z::clear();
		else
			*_reg = *_reg & ~get_gpio_bit(reinterpret_cast<RegisterDataT>(periph));
		[[maybe_unused]] bool delay_after_disabling = is_enabled(periph);
	}
	static bool is_enabled(GPIO_TypeDef *periph) {
		if (periph == GPIOZ)
			return GPIO_Z::read();
		else
			return (*_reg) & get_gpio_bit(reinterpret_cast<RegisterDataT>(periph));
	}
};

/*
s/#define RCC_\(.\{-}\)EN\s\+RCC_\1EN_Msk.*$/using \1 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_typeDef, \1),
\1EN>;/
*/
// using MP_AHB2ENSETR_ADC12 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_typeDef, MP_AHB2ENSETR_ADC12),
// MP_AHB2ENSETR_ADC12EN>;
/*
using ADC_1 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_ADC12EN>;
using ADC_2 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_ADC12EN>;
using ADC_3 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_ADC3EN>;

using ADC = std::variant<ADC_1, ADC_2, ADC_3>;

using SYSCFG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4ENR), RCC_APB4ENR_SYSCFGEN>;

using HSEM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_HSEMEN>;

using SAI_1 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SAI1EN>;
using SAI_2 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SAI2EN>;
using SAI_3 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SAI3EN>;
using SAI_4 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB4ENR_SAI4EN>;

// Todo: this is just here for _attic/templatized_conf_idea.hh
using SAI_variant = std::variant<SAI_1, SAI_2, SAI_3, SAI_4>;

using TIM_1 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM1EN>;
using TIM_2 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM2EN>;
using TIM_3 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM3EN>;
using TIM_4 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM4EN>;
using TIM_5 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM5EN>;
using TIM_6 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM6EN>;
using TIM_7 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM7EN>;
using TIM_8 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM8EN>;
using TIM_9 = NonexistantRegister;
using TIM_10 = NonexistantRegister;
using TIM_11 = NonexistantRegister;
using TIM_12 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM12EN>;
using TIM_13 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM13EN>;
using TIM_14 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM14EN>;
using TIM_15 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM15EN>;
using TIM_16 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM16EN>;
using TIM_17 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM17EN>;

using DMA_1 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_DMA1EN>;
using DMA_2 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_DMA2EN>;
using BDMA_P = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_BDMAEN>;
using MDMA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB3ENR), RCC_AHB3ENR_MDMAEN>;

using DMA2D_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB3ENR), RCC_AHB3ENR_DMA2DEN>;

using I2C_1 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_I2C1EN>;
using I2C_2 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_I2C2EN>;
using I2C_3 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_I2C3EN>;

using SPI_1 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SPI1EN>;
using SPI_2 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_SPI2EN>;
using SPI_3 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_SPI3EN>;
using SPI_4 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SPI4EN>;
using SPI_5 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SPI5EN>;
using SPI_6 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4ENR), RCC_APB4ENR_SPI6EN>;

template<unsigned N>
struct SPI {
	using Reg = void;
};
template<>
struct SPI<1> {
	using Reg = SPI_1;
};
template<>
struct SPI<2> {
	using Reg = SPI_2;
};
template<>
struct SPI<3> {
	using Reg = SPI_3;
};
template<>
struct SPI<4> {
	using Reg = SPI_4;
};
template<>
struct SPI<5> {
	using Reg = SPI_5;
};
template<>
struct SPI<6> {
	using Reg = SPI_6;
};
*/
} // namespace RCC_Control

namespace RCC_Reset
{
/*
using GPIO_A = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_GPIOARST>;
using GPIO_B = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_GPIOBRST>;
using GPIO_C = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_GPIOCRST>;
using GPIO_D = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_GPIODRST>;
using GPIO_E = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_GPIOERST>;
using GPIO_F = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_GPIOFRST>;
using GPIO_G = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_GPIOGRST>;
using GPIO_H = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_GPIOHRST>;
using GPIO_I = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_GPIOIRST>;
using GPIO_J = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_GPIOJRST>;

using ADC_1 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR), RCC_AHB1RSTR_ADC12RST>;
using ADC_2 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR), RCC_AHB1RSTR_ADC12RST>;
using ADC_3 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_ADC3RST>;

using SYSCFG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4RSTR), RCC_APB4RSTR_SYSCFGRST>;

using HSEM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_HSEMRST>;

using SAI_1 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_SAI1RST>;
using SAI_2 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_SAI2RST>;
using SAI_3 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_SAI3RST>;
using SAI_4 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB4RSTR_SAI4RST>;

using TIM_1 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_TIM1RST>;
using TIM_2 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_TIM2RST>;
using TIM_3 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_TIM3RST>;
using TIM_4 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_TIM4RST>;
using TIM_5 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_TIM5RST>;
using TIM_6 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_TIM6RST>;
using TIM_7 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_TIM7RST>;
using TIM_8 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_TIM8RST>;
using TIM_9 = NonexistantRegister;
using TIM_10 = NonexistantRegister;
using TIM_11 = NonexistantRegister;
using TIM_12 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_TIM12RST>;
using TIM_13 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_TIM13RST>;
using TIM_14 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_TIM14RST>;
using TIM_15 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_TIM15RST>;
using TIM_16 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_TIM16RST>;
using TIM_17 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_TIM17RST>;

using DMA_1 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR), RCC_AHB1RSTR_DMA1RST>;
using DMA_2 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR), RCC_AHB1RSTR_DMA2RST>;
using BDMA_P = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_BDMARST>;
using MDMA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB3RSTR), RCC_AHB3RSTR_MDMARST>;

using I2C_1 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_I2C1RST>;
using I2C_2 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_I2C2RST>;
using I2C_3 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_I2C3RST>;

using SPI_1 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_SPI1RST>;
using SPI_2 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_SPI2RST>;
using SPI_3 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_SPI3RST>;
using SPI_4 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_SPI4RST>;
using SPI_5 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_SPI5RST>;
using SPI_6 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4RSTR), RCC_APB4RSTR_SPI6RST>;

*/
} // namespace RCC_Reset

namespace RCC_Clocks
{
/*
using CR = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, CR), 0xFFFFFFFF>;
using D1CKREADY = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, CR), RCC_CR_D1CKRDY>;
using D2CKREADY = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, CR), RCC_CR_D2CKRDY>;
*/
} // namespace RCC_Clocks

} // namespace core_a7
} // namespace stm32mp1
} // namespace mdrivlib
