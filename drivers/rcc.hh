#pragma once
#include "stm32xx.h"

namespace mdrivlib
{
using RegisterT = uint32_t;

void _set_bit_in_struct_by_offset(uint8_t *base, RegisterT offset, RegisterT bit) {
	*(RegisterT *)(base + offset) |= bit;
}

void _clear_bit_in_struct_by_offset(uint8_t *base, RegisterT offset, RegisterT bit) {
	*(RegisterT *)(base + offset) &= ~bit;
}

RegisterT _read_bit_in_struct_by_offset(uint8_t *base, RegisterT offset, RegisterT bit) {
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

#ifdef STM32H7
// stm32h755xx.h:

template<typename PeriphT>
struct RCCPeriph {
	void enable(RegisterT bit) { *PeriphT::_reg |= bit; }
	// RCC->*PeriphT::rcc_enabled_field |= bit;
	void disable(RegisterT bit) { *PeriphT::_reg &= ~bit; }
	bool is_enabled(RegisterT bit) { return *PeriphT::_reg & bit; }
};

struct RCCControl {
	struct GPIO {
		static inline volatile RegisterT *const _reg = &(RCC->AHB4ENR);
		static void enable(uint32_t periph_base_addr) { *_reg |= (periph_base_addr >> 10); }
		static void disable(uint32_t periph_base_addr) { *_reg &= ~(periph_base_addr >> 10); }
		static bool is_enabled(uint32_t periph_base_addr) { return (*_reg) & (periph_base_addr >> 10); }
	};
	struct ADC_1 : public RCCPeriph<ADC_1> {
		// static inline volatile uint32_t RCC_TypeDef::*rcc_enable_field = &RCC_TypeDef::AHB1ENR;
		volatile static inline RegisterT *const Reg = &(RCC->AHB1ENR);
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
};

enum class GPIOEnableBit : uint32_t {
	A = RCC_AHB4ENR_GPIOAEN,
	B = RCC_AHB4ENR_GPIOBEN,
	C = RCC_AHB4ENR_GPIOCEN,
	D = RCC_AHB4ENR_GPIODEN,
	E = RCC_AHB4ENR_GPIOEEN,
	F = RCC_AHB4ENR_GPIOFEN,
	G = RCC_AHB4ENR_GPIOGEN,
	H = RCC_AHB4ENR_GPIOHEN,
	I = RCC_AHB4ENR_GPIOIEN,
	J = RCC_AHB4ENR_GPIOJEN,
	K = RCC_AHB4ENR_GPIOKEN,
	reg = offsetof(RCC_TypeDef, AHB4ENR),
};
// struct GPIOEnableBit {
//	const static inline uint32_t A = RCC_AHB4ENR_GPIOAEN;
//	const static inline uint32_t B = RCC_AHB4ENR_GPIOBEN;
//	const static inline uint32_t C = RCC_AHB4ENR_GPIOCEN;
//	const static inline uint32_t D = RCC_AHB4ENR_GPIODEN;
//	const static inline uint32_t E = RCC_AHB4ENR_GPIOEEN;
//	const static inline uint32_t F = RCC_AHB4ENR_GPIOFEN;
//	const static inline uint32_t G = RCC_AHB4ENR_GPIOGEN;
//	const static inline uint32_t H = RCC_AHB4ENR_GPIOHEN;
//	const static inline uint32_t I = RCC_AHB4ENR_GPIOIEN;
//	const static inline uint32_t J = RCC_AHB4ENR_GPIOJEN;
//	const static inline uint32_t K = RCC_AHB4ENR_GPIOKEN;
//	//const static inline uint32_t offset = offsetof(RCC_TypeDef, AHB4ENR);
//	static inline volatile uint32_t * const rcc_enable = &(RCC->AHB4ENR);
//	static inline volatile uint32_t RCC_TypeDef::* rcc_enable_field = &RCC_TypeDef::AHB4ENR;
//};

// __IO uint32_t RCCType_Def::* GPIORCCReg = &RCC_TypeDef::AHB4ENR;
// bool _is_enabled(GPIOEnableBit en_bit) { return _read_bit(RCC->*GPIORCCReg, en_bit); }
// void _enable(GPIOEnableBit en_bit) { _set_bit(RCC->*GPIORCCReg, en_bit); }
// void _disable(GPIOEnableBit en_bit) { _clear_bit(RCC->*GPIORCCReg, en_bit); }

enum class ADCEnableBit : uint32_t {
	_1 = RCC_AHB1ENR_ADC12EN,
	_2 = RCC_AHB1ENR_ADC12EN,
	reg = offsetof(RCC_TypeDef, AHB1ENR),
};

enum class ADC3EnableBit : uint32_t {
	_3 = RCC_AHB4ENR_ADC3EN,
	reg = offsetof(RCC_TypeDef, AHB1ENR),
};

enum class DMAEnableBit : uint32_t {
	_1 = RCC_AHB1ENR_DMA1EN,
	_2 = RCC_AHB1ENR_DMA2EN,
	reg = offsetof(RCC_TypeDef, AHB1ENR),
};

enum class I2CEnableBit : uint32_t {
	_1 = RCC_APB1LENR_I2C1EN,
	_2 = RCC_APB1LENR_I2C2EN,
	_3 = RCC_APB1LENR_I2C3EN,
	reg = offsetof(RCC_TypeDef, APB1LENR),
};

enum class SAIEnableBit : uint32_t {
	_1 = RCC_APB2ENR_SAI1EN,
	_2 = RCC_APB2ENR_SAI2EN,
	_3 = RCC_APB2ENR_SAI3EN,
	reg = offsetof(RCC_TypeDef, APB2ENR),
};

enum class SAI4EnableBit : uint32_t {
	_4 = RCC_APB4ENR_SAI4EN,
	reg = offsetof(RCC_TypeDef, APB4ENR),
};
#endif

#ifdef STM32F4
// Todo: Can we just copy F7 values?
#endif

#ifdef STM32F7
// stm32f746xx.h:
class GPIOEnableBit {
	const static inline uint32_t A = RCC_AHB1ENR_GPIOAEN;
	const static inline uint32_t B = RCC_AHB1ENR_GPIOBEN;
	const static inline uint32_t C = RCC_AHB1ENR_GPIOCEN;
	const static inline uint32_t D = RCC_AHB1ENR_GPIODEN;
	const static inline uint32_t E = RCC_AHB1ENR_GPIOEEN;
	const static inline uint32_t F = RCC_AHB1ENR_GPIOFEN;
	const static inline uint32_t G = RCC_AHB1ENR_GPIOGEN;
	const static inline uint32_t H = RCC_AHB1ENR_GPIOHEN;
	const static inline uint32_t I = RCC_AHB1ENR_GPIOIEN;
	const static inline uint32_t J = RCC_AHB1ENR_GPIOJEN;
	const static inline uint32_t K = RCC_AHB1ENR_GPIOKEN;
	constexpr static inline offset = offsetof(RCC_TypeDef, AHB1ENR);
};
bool _is_enabled(GPIOEnableBit en_bit) { return _read_bit(RCC->AHB1ENR, en_bit); }
void _enable(GPIOEnableBit en_bit) { _set_bit(RCC->AHB1ENR, en_bit); }
void _disable(GPIOEnableBit en_bit) { _clear_bit(RCC->AHB1ENR, en_bit); }

enum class ADCEnableBit : uint32_t {
	_1 = RCC_APB2ENR_ADC1EN,
	_2 = RCC_APB2ENR_ADC2EN,
	_3 = RCC_APB2ENR_ADC3EN,
};
bool _is_enabled(ADCEnableBit en_bit) { return _read_bit(RCC->APB2ENR, en_bit); }
void _enable(ADCEnableBit en_bit) { _set_bit(RCC->APB2ENR, en_bit); }
void _disable(ADCEnableBit en_bit) { _clear_bit(RCC->APB2ENR, en_bit); }

enum class DMAEnableBit : uint32_t {
	_1 = RCC_AHB1ENR_DMA1EN,
	_2 = RCC_AHB1ENR_DMA2EN,
};
bool _is_enabled(DMAEnableBit en_bit) { return _read_bit(RCC->AHB1ENR, en_bit); }
void _enable(DMAEnableBit en_bit) { _set_bit(RCC->AHB1ENR, en_bit); }
void _disable(DMAEnableBit en_bit) { _clear_bit(RCC->AHB1ENR, en_bit); }

enum class I2CEnableBit : uint32_t {
	_1 = RCC_APB1ENR_I2C1EN,
	_2 = RCC_APB1ENR_I2C2EN,
	_3 = RCC_APB1ENR_I2C3EN,
};
bool _is_enabled(I2CEnableBit en_bit) { return _read_bit(RCC->APB1ENR, en_bit); }
void _enable(I2CEnableBit en_bit) { _set_bit(RCC->APB1ENR, en_bit); }
void _disable(I2CEnableBit en_bit) { _clear_bit(RCC->APB1ENR, en_bit); }

enum class SAIEnableBit : uint32_t {
	_1 = RCC_APB2ENR_SAI1EN,
	_2 = RCC_APB2ENR_SAI2EN,
	_3 = RCC_APB2ENR_SAI3EN,
};
enum class SAI4EnableBit : uint32_t {
	_4 = RCC_APB4ENR_SAI4EN,
};
__IO uint32_t RCCTypeDef::*SAIRCCReg = &RCC_TypeDef::APB2ENR;
__IO uint32_t RCCTypeDef::*SAI4RCCReg = &RCC_TypeDef::APB4ENR;
bool _is_enabled(SAIEnableBit en_bit) { return _read_bit(RCC->*SAIRCCReg, en_bit); }
void _enable(SAIEnableBit en_bit) { _set_bit(RCC->*SAIRCCReg, en_bit); }
void _disable(SAIEnableBit en_bit) { _clear_bit(RCC->*SAIRCCReg, en_bit); }
bool _is_enabled(SAI4EnableBit en_bit) { return _read_bit(RCC->*SAI4RCCReg, en_bit); }
void _enable(SAI4EnableBit en_bit) { _set_bit(RCC->*SAI4RCCReg, en_bit); }
void _disable(SAI4EnableBit en_bit) { _clear_bit(RCC->*SAI4RCCReg, en_bit); }

enum class SAIEnableBit : uint32_t {
	_1 = RCC_APB2ENR_SAI1EN,
	_2 = RCC_APB2ENR_SAI2EN,
	_3 = RCC_APB2ENR_SAI3EN,
};
__IO uint32_t RCCTypeDef::*SAIRCCReg = &RCC_TypeDef::APB2ENR;
bool _is_enabled(SAIEnableBit en_bit) { return _read_bit(RCC->*SAIRCCReg, en_bit); }
void _enable(SAIEnableBit en_bit) { _set_bit(RCC->*SAIRCCReg, en_bit); }
void _disable(SAIEnableBit en_bit) { _clear_bit(RCC->*SAIRCCReg, en_bit); }

#endif

#ifdef STM32MP1
	#ifdef CORE_CM4

	#endif
#endif

} // namespace mdrivlib
