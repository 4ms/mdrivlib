#pragma once
#include "register_access.hh"
#include "stm32xx.h"

namespace mdrivlib
{

#ifdef STM32H7
namespace stm32h7x5
{
namespace RCC_Control
{
using RegisterDataT = uint32_t;

using GPIO_A = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_GPIOAEN>;
using GPIO_B = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_GPIOBEN>;
using GPIO_C = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_GPIOCEN>;
using GPIO_D = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_GPIODEN>;
using GPIO_E = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_GPIOEEN>;
using GPIO_F = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_GPIOFEN>;
using GPIO_G = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_GPIOGEN>;
using GPIO_H = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_GPIOHEN>;
using GPIO_I = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_GPIOIEN>;
using GPIO_J = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_GPIOJEN>;
// special-case: GPIO port base address can be used to calc bit-offset of RCC enable bit
struct GPIO {
	static inline volatile RegisterDataT *const _reg = &(RCC->AHB4ENR);

	static uint32_t get_gpio_bit(RegisterDataT periph) {
		return 1 << ((periph & 0x00003C00) >> 10);
	}
	static void enable(GPIO_TypeDef *periph) {
		*_reg = *_reg | get_gpio_bit(reinterpret_cast<RegisterDataT>(periph));
		[[maybe_unused]] bool delay_after_enabling = is_enabled(periph);
	}
	static void disable(GPIO_TypeDef *periph) {
		*_reg = *_reg & ~get_gpio_bit(reinterpret_cast<RegisterDataT>(periph));
		[[maybe_unused]] bool delay_after_disabling = is_enabled(periph);
	}
	static bool is_enabled(GPIO_TypeDef *periph) {
		return (*_reg) & get_gpio_bit(reinterpret_cast<RegisterDataT>(periph));
	}
};

using ADC_1 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_ADC12EN>;
using ADC_2 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_ADC12EN>;
using ADC_3 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_ADC3EN>;

using SYSCFG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4ENR), RCC_APB4ENR_SYSCFGEN>;

using SAI_1 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SAI1EN>;
using SAI_2 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SAI2EN>;
using SAI_3 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SAI3EN>;
using SAI_4 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB4ENR_SAI4EN>;

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

using I2C_1 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_I2C1EN>;
using I2C_2 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_I2C2EN>;
using I2C_3 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_I2C3EN>;
} // namespace RCC_Control
} // namespace stm32h7x5

#endif

#ifdef STM32F4
// Todo: Can we just copy F7 values?
#endif

#ifdef STM32F7
// stm32f746xx.h:

// Todo: convert this to new format:
// enum class ADCEnableBit : uint32_t {
// 	_1 = RCC_APB2ENR_ADC1EN,
// 	_2 = RCC_APB2ENR_ADC2EN,
// 	_3 = RCC_APB2ENR_ADC3EN,
// };
// enum class DMAEnableBit : uint32_t {
// 	_1 = RCC_AHB1ENR_DMA1EN,
// 	_2 = RCC_AHB1ENR_DMA2EN,
// };
// enum class I2CEnableBit : uint32_t {
// 	_1 = RCC_APB1ENR_I2C1EN,
// 	_2 = RCC_APB1ENR_I2C2EN,
// 	_3 = RCC_APB1ENR_I2C3EN,
// };
// enum class SAIEnableBit : uint32_t {
// 	_1 = RCC_APB2ENR_SAI1EN,
// 	_2 = RCC_APB2ENR_SAI2EN,
// 	_3 = RCC_APB2ENR_SAI3EN,
// };
// enum class SAI4EnableBit : uint32_t {
// 	_4 = RCC_APB4ENR_SAI4EN,
// };

// struct TIM : RCCPeriphs<TIM> {
// 	volatile static inline RegisterT *const _regs[NumP] = {
// 		&RCC->APB2ENR,
// 		&RCC->APB1ENR,
// 		&RCC->APB1ENR,
// 		&RCC->APB1ENR,
// 		&RCC->APB1ENR,
// 		&RCC->APB1ENR,
// 		&RCC->APB1ENR,
// 		&RCC->APB2ENR,
// 		&RCC->APB2ENR,
// 		&RCC->APB2ENR,
// 		&RCC->APB2ENR,
// 		&RCC->APB1ENR,
// 		&RCC->APB1ENR,
// 		&RCC->APB1ENR,
// 		&RCC->APB2ENR,
// 		&RCC->APB2ENR,
// 		&RCC->APB2ENR,
// 	};
// 	const static inline RegisterT _bits[NumP] = {
// 		RCC_APB2ENR_TIM1EN,
// 		RCC_APB1ENR_TIM2EN,
// 		RCC_APB1ENR_TIM3EN,
// 		RCC_APB1ENR_TIM4EN,
// 		RCC_APB1ENR_TIM5EN,
// 		RCC_APB1ENR_TIM6EN,
// 		RCC_APB1ENR_TIM7EN,
// 		RCC_APB2ENR_TIM8EN,
// 		RCC_APB2ENR_TIM9EN,
// 		RCC_APB2ENR_TIM10EN,
// 		RCC_APB2ENR_TIM11EN,
// 		RCC_APB1ENR_TIM12EN,
// 		RCC_APB1ENR_TIM13EN,
// 		RCC_APB1ENR_TIM14EN,
// 		RCC_APB2ENR_TIM15EN,
// 		RCC_APB2ENR_TIM16EN,
// 		RCC_APB2ENR_TIM17EN,
// 	};
// };

#endif

#ifdef STM32MP1
#ifdef CORE_CM4

#endif
#endif

} // namespace mdrivlib

