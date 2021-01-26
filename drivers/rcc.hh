#pragma once
#include "stm32xx.h"

namespace mdrivlib
{
using RegisterT = uint32_t;

static inline void _set_bit_in_struct_by_offset(uint8_t *base, RegisterT offset, RegisterT bit) {
	*(RegisterT *)(base + offset) |= bit;
}

static inline void _clear_bit_in_struct_by_offset(uint8_t *base, RegisterT offset, RegisterT bit) {
	*(RegisterT *)(base + offset) &= ~bit;
}

static inline RegisterT _read_bit_in_struct_by_offset(uint8_t *base, RegisterT offset, RegisterT bit) {
	return (*(RegisterT *)(base + offset)) & bit;
}

template<typename T>
void enable_rcc(T en_bit) {
	_set_bit_in_struct_by_offset(
		(uint8_t *)RCC, static_cast<volatile RegisterT>(T::reg), static_cast<RegisterT>(en_bit));
}

template<typename T>
void disable_rcc(T en_bit) {
	_clear_bit_in_struct_by_offset(
		(uint8_t *)RCC, static_cast<volatile RegisterT>(T::reg), static_cast<RegisterT>(en_bit));
}

template<typename T>
bool is_enabled_rcc(T en_bit) {
	return _read_bit_in_struct_by_offset(
		(uint8_t *)RCC, static_cast<volatile RegisterT>(T::reg), static_cast<RegisterT>(en_bit));
}

template<typename PeriphT>
struct RCCPeriph {
	static void enable() { *PeriphT::_reg |= PeriphT::_bit; }
	static void disable() { *PeriphT::_reg &= ~PeriphT::_bit; }
	static bool is_enabled() { return *PeriphT::_reg & PeriphT::_bit; }
};

template<typename PeriphT>
struct RCCPeriphs {
	static void enable(unsigned pnum) { *PeriphT::_regs[pnum - 1] |= PeriphT::_bits[pnum - 1]; }
	static void disable(unsigned pnum) { *PeriphT::_regs[pnum - 1] &= ~PeriphT::_bits[pnum - 1]; }
	static bool is_enabled(unsigned pnum) { return *PeriphT::_regs[pnum - 1] & PeriphT::_bits[pnum - 1]; }
};

#ifdef STM32H7
// stm32h755xx.h:

struct RCCControl {
	// special-case: GPIO port base address can be used to calc bit-offset of RCC enable bit
	struct GPIO {
		static inline volatile RegisterT *const _reg = &(RCC->AHB4ENR);
		static uint32_t get_gpio_bit(uint32_t periph_base_addr) { return (periph_base_addr & 0x00003C00) >> 10; }
		static void enable(uint32_t periph_base_addr) { *_reg |= get_gpio_bit(periph_base_addr); }
		static void disable(uint32_t periph_base_addr) { *_reg &= ~get_gpio_bit(periph_base_addr); }
		static bool is_enabled(uint32_t periph_base_addr) { return (*_reg) & get_gpio_bit(periph_base_addr); }
	};

	struct ADC : RCCPeriphs<ADC> {
		const static inline unsigned NumP = 3;
		volatile static inline RegisterT *const _regs[NumP] = {
			&RCC->AHB1ENR,
			&RCC->AHB1ENR,
			&RCC->AHB1ENR,
		};
		const static inline RegisterT _bits[NumP] = {
			RCC_AHB1ENR_ADC12EN,
			RCC_AHB1ENR_ADC12EN,
			RCC_AHB4ENR_ADC3EN,
		};
	};

	struct ADC_1 : public RCCPeriph<ADC_1> {
		volatile static inline RegisterT *const _reg = &(RCC->AHB1ENR);
		const static inline RegisterT _bit = RCC_AHB1ENR_ADC12EN;
	};
	struct ADC_2 : public RCCPeriph<ADC_2> {
		volatile static inline RegisterT *const _reg = &(RCC->AHB1ENR);
		const static inline RegisterT _bit = RCC_AHB1ENR_ADC12EN;
	};
	struct ADC_3 : public RCCPeriph<ADC_3> {
		volatile static inline RegisterT *const _reg = &(RCC->AHB1ENR);
		const static inline RegisterT _bit = RCC_AHB4ENR_ADC3EN;
	};

	// Todo: decide if the ADC : RCCPeriphs<ADC> method or the ADC_1 : RCCPeriph<ADC_1> method is better, and apply to
	// all (including GPIO):
	struct DMA_1 : public RCCPeriph<DMA_1> {
		volatile static inline RegisterT *const _reg = &(RCC->AHB1ENR);
		const static inline RegisterT _bit = RCC_AHB1ENR_DMA1EN;
	};
	struct DMA_2 : public RCCPeriph<DMA_2> {
		volatile static inline RegisterT *const _reg = &(RCC->AHB1ENR);
		const static inline RegisterT _bit = RCC_AHB1ENR_DMA2EN;
	};

	struct I2C_1 : public RCCPeriph<I2C_1> {
		volatile static inline RegisterT *const _reg = &(RCC->APB1LENR);
		const static inline RegisterT _bit = RCC_APB1LENR_I2C1EN;
	};
	struct I2C_2 : public RCCPeriph<I2C_2> {
		volatile static inline RegisterT *const _reg = &(RCC->APB1LENR);
		const static inline RegisterT _bit = RCC_APB1LENR_I2C2EN;
	};
	struct I2C_3 : public RCCPeriph<I2C_3> {
		volatile static inline RegisterT *const _reg = &(RCC->APB1LENR);
		const static inline RegisterT _bit = RCC_APB1LENR_I2C3EN;
	};

	struct SAI_1 : public RCCPeriph<SAI_1> {
		volatile static inline RegisterT *const _reg = &(RCC->APB2ENR);
		const static inline RegisterT _bit = RCC_APB2ENR_SAI1EN;
	};
	struct SAI_2 : public RCCPeriph<SAI_2> {
		volatile static inline RegisterT *const _reg = &(RCC->APB2ENR);
		const static inline RegisterT _bit = RCC_APB2ENR_SAI2EN;
	};
	struct SAI_3 : public RCCPeriph<SAI_3> {
		volatile static inline RegisterT *const _reg = &(RCC->APB2ENR);
		const static inline RegisterT _bit = RCC_APB2ENR_SAI3EN;
	};
	struct SAI_4 : public RCCPeriph<SAI_4> {
		volatile static inline RegisterT *const _reg = &(RCC->APB4ENR);
		const static inline RegisterT _bit = RCC_APB4ENR_SAI4EN;
	};

	struct TIM : RCCPeriphs<TIM> {
		const static inline unsigned NumP = 17;
		volatile static inline RegisterT *const _regs[NumP] = {
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
		const static inline RegisterT _bits[NumP] = {
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
	static uint32_t get_gpio_bit(uint32_t periph_base_addr) { return (periph_base_addr & 0x00003C00) >> 10; }
	static void enable(uint32_t periph_base_addr) { *_reg |= get_gpio_bit(periph_base_addr); }
	static void disable(uint32_t periph_base_addr) { *_reg &= ~get_gpio_bit(periph_base_addr); }
	static bool is_enabled(uint32_t periph_base_addr) { return (*_reg) & get_gpio_bit(periph_base_addr); }
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
