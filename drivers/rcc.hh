#pragma once
#include "register_access.hh"
#include "stm32xx.h"

namespace mdrivlib
{
using RegisterDataT = uint32_t;

/*
static inline void _set_bit_in_struct_by_offset(uint8_t *base, RegisterDataT offset, RegisterDataT bit) {
	*(RegisterDataT *)(base + offset) |= bit;
}

static inline void _clear_bit_in_struct_by_offset(uint8_t *base, RegisterDataT offset, RegisterDataT bit) {
	*(RegisterDataT *)(base + offset) &= ~bit;
}

static inline RegisterDataT _read_bit_in_struct_by_offset(uint8_t *base, RegisterDataT offset, RegisterDataT bit) {
	return (*(RegisterDataT *)(base + offset)) & bit;
}

template<typename T>
void enable_rcc(T en_bit) {
	_set_bit_in_struct_by_offset(
		(uint8_t *)RCC, static_cast<volatile RegisterDataT>(T::reg), static_cast<RegisterDataT>(en_bit));
}

template<typename T>
void disable_rcc(T en_bit) {
	_clear_bit_in_struct_by_offset(
		(uint8_t *)RCC, static_cast<volatile RegisterDataT>(T::reg), static_cast<RegisterDataT>(en_bit));
}

template<typename T>
bool is_enabled_rcc(T en_bit) {
	return _read_bit_in_struct_by_offset(
		(uint8_t *)RCC, static_cast<volatile RegisterDataT>(T::reg), static_cast<RegisterDataT>(en_bit));
}
*/

// Method #1:
template<typename PeriphT>
struct RCCPeriph {
	static void enable() {
		*PeriphT::_reg |= PeriphT::_bit;
	}
	static void disable() {
		*PeriphT::_reg &= ~PeriphT::_bit;
	}
	static bool is_enabled() {
		return *PeriphT::_reg & PeriphT::_bit;
	}
};

// Method #2:
template<typename PeriphT>
struct RCCPeriphs {
	static void enable(unsigned pnum) {
		*PeriphT::_regs[pnum - 1] |= PeriphT::_bits[pnum - 1];
	}
	static void disable(unsigned pnum) {
		*PeriphT::_regs[pnum - 1] &= ~PeriphT::_bits[pnum - 1];
	}
	static bool is_enabled(unsigned pnum) {
		return *PeriphT::_regs[pnum - 1] & PeriphT::_bits[pnum - 1];
	}
};

// Method #3:
struct EnableFlag {
	volatile RegisterDataT *const reg;
	const RegisterDataT bit;
};

template<typename PeriphT>
struct RCCPeriphControl {
	static void enable(unsigned pnum = 1) {
		*PeriphT::_enableflags[pnum - 1].reg |= PeriphT::_enableflags[pnum - 1].bit;
	}
	static void disable(unsigned pnum = 1) {
		*PeriphT::_enableflags[pnum - 1].reg &= ~PeriphT::_enableflags[pnum - 1].bit;
	}
	static bool is_enabled(unsigned pnum = 1) {
		return *PeriphT::_enableflags[pnum - 1].reg & PeriphT::_enableflags[pnum - 1].bit;
	}
};

#ifdef STM32H7
// stm32h755xx.h:

// Method #4:
//(uses register_access.hh)
namespace stm32h7x5xx
{
namespace RCC_Control
{
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
	const static inline uint32_t NumPeriph = 10;

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
struct ADC {
	const static inline uint32_t NumPeriph = 3;
};

using SYSCFG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4ENR), RCC_APB4ENR_SYSCFGEN>;

using SAI_1 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SAI1EN>;
using SAI_2 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SAI2EN>;
using SAI_3 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SAI3EN>;
using SAI_4 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB4ENR_SAI4EN>;
struct SAI {
	const static inline uint32_t NumPeriph = 4;
};

struct TIM {
	const static inline unsigned NumPerpih = 17;
};

using TIM_1 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM1EN>;
using TIM_2 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM2EN>;
using TIM_3 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM3EN>;
using TIM_4 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM4EN>;
using TIM_5 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM5EN>;
using TIM_6 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM6EN>;
using TIM_7 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM7EN>;
using TIM_8 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM8EN>;
using TIM_12 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM12EN>;
using TIM_13 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM13EN>;
using TIM_14 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM14EN>;
using TIM_15 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM15EN>;
using TIM_16 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM16EN>;
using TIM_17 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM17EN>;
} // namespace RCC_Control
} // namespace stm32h7x5xx

// Methods 1-3:
struct RCCControl {
	// special-case: GPIO port base address can be used to calc bit-offset of RCC enable bit
	struct GPIO {
		static inline volatile RegisterDataT *const _reg = &(RCC->AHB4ENR);
		static uint32_t get_gpio_bit(uint32_t periph_base_addr) {
			return 1 << ((periph_base_addr & 0x00003C00) >> 10);
		}
		static void enable(uint32_t periph_base_addr) {
			// std::atomic
			auto tmp = *_reg;
			tmp |= get_gpio_bit(periph_base_addr);
			*_reg = tmp;
			// end
		}
		static void disable(uint32_t periph_base_addr) {
			// std::atomic
			auto tmp = *_reg;
			tmp &= ~get_gpio_bit(periph_base_addr);
			*_reg = tmp;
			// end
		}
		static bool is_enabled(uint32_t periph_base_addr) {
			return (*_reg) & get_gpio_bit(periph_base_addr);
		}
	};

	struct SYS_CFG : RCCPeriphControl<SYS_CFG> {
		const static inline unsigned NumP = 1;
		static inline EnableFlag _enableflags[NumP] = {
			{&RCC->APB4ENR, RCC_APB4ENR_SYSCFGEN},
		};
	};

	struct BDMA_P : RCCPeriphControl<BDMA_P> {
		const static inline unsigned NumP = 1;
		static inline EnableFlag _enableflags[NumP] = {
			{&RCC->AHB4ENR, RCC_AHB4ENR_BDMAEN},
		};
	};

	struct ADC : RCCPeriphs<ADC> {
		const static inline unsigned NumP = 3;
		volatile static inline RegisterDataT *const _regs[NumP] = {
			&RCC->AHB1ENR,
			&RCC->AHB1ENR,
			&RCC->AHB4ENR,
		};
		const static inline RegisterDataT _bits[NumP] = {
			RCC_AHB1ENR_ADC12EN,
			RCC_AHB1ENR_ADC12EN,
			RCC_AHB4ENR_ADC3EN,
		};
	};

	struct ADC_1 : public RCCPeriph<ADC_1> {
		volatile static inline RegisterDataT *const _reg = &(RCC->AHB1ENR);
		const static inline RegisterDataT _bit = RCC_AHB1ENR_ADC12EN;
	};
	struct ADC_2 : public RCCPeriph<ADC_2> {
		volatile static inline RegisterDataT *const _reg = &(RCC->AHB1ENR);
		const static inline RegisterDataT _bit = RCC_AHB1ENR_ADC12EN;
	};
	struct ADC_3 : public RCCPeriph<ADC_3> {
		volatile static inline RegisterDataT *const _reg = &(RCC->AHB4ENR);
		const static inline RegisterDataT _bit = RCC_AHB4ENR_ADC3EN;
	};

	// Todo: decide if the ADC : RCCPeriphs<ADC> method or the ADC_1 : RCCPeriph<ADC_1> method is better, and apply to
	// all (including GPIO):
	struct DMA_1 : public RCCPeriph<DMA_1> {
		volatile static inline RegisterDataT *const _reg = &(RCC->AHB1ENR);
		const static inline RegisterDataT _bit = RCC_AHB1ENR_DMA1EN;
	};
	struct DMA_2 : public RCCPeriph<DMA_2> {
		volatile static inline RegisterDataT *const _reg = &(RCC->AHB1ENR);
		const static inline RegisterDataT _bit = RCC_AHB1ENR_DMA2EN;
	};

	struct I2C_1 : public RCCPeriph<I2C_1> {
		volatile static inline RegisterDataT *const _reg = &(RCC->APB1LENR);
		const static inline RegisterDataT _bit = RCC_APB1LENR_I2C1EN;
	};
	struct I2C_2 : public RCCPeriph<I2C_2> {
		volatile static inline RegisterDataT *const _reg = &(RCC->APB1LENR);
		const static inline RegisterDataT _bit = RCC_APB1LENR_I2C2EN;
	};
	struct I2C_3 : public RCCPeriph<I2C_3> {
		volatile static inline RegisterDataT *const _reg = &(RCC->APB1LENR);
		const static inline RegisterDataT _bit = RCC_APB1LENR_I2C3EN;
	};

	struct SAI_1 : public RCCPeriph<SAI_1> {
		volatile static inline RegisterDataT *const _reg = &(RCC->APB2ENR);
		const static inline RegisterDataT _bit = RCC_APB2ENR_SAI1EN;
	};
	struct SAI_2 : public RCCPeriph<SAI_2> {
		volatile static inline RegisterDataT *const _reg = &(RCC->APB2ENR);
		const static inline RegisterDataT _bit = RCC_APB2ENR_SAI2EN;
	};
	struct SAI_3 : public RCCPeriph<SAI_3> {
		volatile static inline RegisterDataT *const _reg = &(RCC->APB2ENR);
		const static inline RegisterDataT _bit = RCC_APB2ENR_SAI3EN;
	};
	struct SAI_4 : public RCCPeriph<SAI_4> {
		volatile static inline RegisterDataT *const _reg = &(RCC->APB4ENR);
		const static inline RegisterDataT _bit = RCC_APB4ENR_SAI4EN;
	};

	struct TIM : RCCPeriphs<TIM> {
		const static inline unsigned NumP = 17;
		volatile static inline RegisterDataT *const _regs[NumP] = {
			&RCC->APB2ENR,
			&RCC->APB1LENR,
			&RCC->APB1LENR,
			&RCC->APB1LENR,
			&RCC->APB1LENR,
			&RCC->APB1LENR,
			&RCC->APB1LENR,
			&RCC->APB2ENR,
			&RCC->APB2ENR, // TIM9 is not present: map to TIM1
			&RCC->APB2ENR, // TIM10 is not present: map to TIM1
			&RCC->APB2ENR, // TIM11 is not present: map to TIM1
			&RCC->APB1LENR,
			&RCC->APB1LENR,
			&RCC->APB1LENR,
			&RCC->APB2ENR,
			&RCC->APB2ENR,
			&RCC->APB2ENR,
		};
		const static inline RegisterDataT _bits[NumP] = {
			RCC_APB2ENR_TIM1EN,
			RCC_APB1LENR_TIM2EN,
			RCC_APB1LENR_TIM3EN,
			RCC_APB1LENR_TIM4EN,
			RCC_APB1LENR_TIM5EN,
			RCC_APB1LENR_TIM6EN,
			RCC_APB1LENR_TIM7EN,
			RCC_APB2ENR_TIM8EN,
			RCC_APB2ENR_TIM1EN, //?? TIM9 is not present: map to TIM1
			RCC_APB2ENR_TIM1EN, //?? TIM10 is not present: map to TIM1
			RCC_APB2ENR_TIM1EN, //?? TIM11 is not present: map to TIM1
			RCC_APB1LENR_TIM12EN,
			RCC_APB1LENR_TIM13EN,
			RCC_APB1LENR_TIM14EN,
			RCC_APB2ENR_TIM15EN,
			RCC_APB2ENR_TIM16EN,
			RCC_APB2ENR_TIM17EN,
		};
	};
};

#endif

#ifdef STM32F4
// Todo: Can we just copy F7 values?
#endif

#ifdef STM32F7
// stm32f746xx.h:

struct GPIO {
	static inline volatile RegisterT *const _reg = &(RCC->AHB4ENR);
	static uint32_t get_gpio_bit(uint32_t periph_base_addr) {
		return (periph_base_addr & 0x00003C00) >> 10;
	}
	static void enable(uint32_t periph_base_addr) {
		*_reg |= get_gpio_bit(periph_base_addr);
	}
	static void disable(uint32_t periph_base_addr) {
		*_reg &= ~get_gpio_bit(periph_base_addr);
	}
	static bool is_enabled(uint32_t periph_base_addr) {
		return (*_reg) & get_gpio_bit(periph_base_addr);
	}
};

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

struct TIM : RCCPeriphs<TIM> {
	const static inline unsigned NumP = 17;
	volatile static inline RegisterT *const _regs[NumP] = {
		&RCC->APB2ENR,
		&RCC->APB1ENR,
		&RCC->APB1ENR,
		&RCC->APB1ENR,
		&RCC->APB1ENR,
		&RCC->APB1ENR,
		&RCC->APB1ENR,
		&RCC->APB2ENR,
		&RCC->APB2ENR,
		&RCC->APB2ENR,
		&RCC->APB2ENR,
		&RCC->APB1ENR,
		&RCC->APB1ENR,
		&RCC->APB1ENR,
		&RCC->APB2ENR,
		&RCC->APB2ENR,
		&RCC->APB2ENR,
	};
	const static inline RegisterT _bits[NumP] = {
		RCC_APB2ENR_TIM1EN,
		RCC_APB1ENR_TIM2EN,
		RCC_APB1ENR_TIM3EN,
		RCC_APB1ENR_TIM4EN,
		RCC_APB1ENR_TIM5EN,
		RCC_APB1ENR_TIM6EN,
		RCC_APB1ENR_TIM7EN,
		RCC_APB2ENR_TIM8EN,
		RCC_APB2ENR_TIM9EN,
		RCC_APB2ENR_TIM10EN,
		RCC_APB2ENR_TIM11EN,
		RCC_APB1ENR_TIM12EN,
		RCC_APB1ENR_TIM13EN,
		RCC_APB1ENR_TIM14EN,
		RCC_APB2ENR_TIM15EN,
		RCC_APB2ENR_TIM16EN,
		RCC_APB2ENR_TIM17EN,
	};
};

#endif

#ifdef STM32MP1
#ifdef CORE_CM4

#endif
#endif

} // namespace mdrivlib

// FixMe: Is this a good idea to always include a "using namespace" file??
#include "arch.hh"

