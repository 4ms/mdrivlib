// clang-format off
namespace mdrivlib {

template<typename T>
_set_bit(volatile uint32_t reg, T bit) { reg |= static_cast<uint32_t>(bit); }

template<typename T>
_clear_bit(volatile uint32_t reg, T bit) { reg &= ~(static_cast<uint32_t>(bit)); }

template<typename T>
_read_bit(volatile uint32_t reg, T bit) { return reg & static_cast<uint32_t>(bit); }

template<typename R, typename T>
void _set_bit_in_RCC_field(R offset, T bit) {
	*(uint32_t *)((uint8_t*)(RCC) + static_cast<volatile uint32_t>(offset)) |= static_cast<uint32_t>(bit);
}

#ifdef STM32F4
	//Todo: Can we just copy F7 values?
#endif

#ifdef STM32F7
	//stm32f746xx.h:
	enum class GPIOEnableBit : uint32_t {
		A = RCC_AHB1ENR_GPIOAEN,
		B = RCC_AHB1ENR_GPIOBEN,
		C = RCC_AHB1ENR_GPIOCEN,
		D = RCC_AHB1ENR_GPIODEN,
		E = RCC_AHB1ENR_GPIOEEN,
		F = RCC_AHB1ENR_GPIOFEN,
		G = RCC_AHB1ENR_GPIOGEN,
		H = RCC_AHB1ENR_GPIOHEN,
		I = RCC_AHB1ENR_GPIOIEN,
		J = RCC_AHB1ENR_GPIOJEN,
		K = RCC_AHB1ENR_GPIOKEN,
		offset = offsetof(RCC_TypeDef, AHB1ENR),
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
	__IO uint32_t RCCTypeDef::* SAIRCCReg = &RCC_TypeDef::APB2ENR;
	__IO uint32_t RCCTypeDef::* SAI4RCCReg = &RCC_TypeDef::APB4ENR;
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
	__IO uint32_t RCCTypeDef::* SAIRCCReg = &RCC_TypeDef::APB2ENR;
	bool _is_enabled(SAIEnableBit en_bit) { return _read_bit(RCC->*SAIRCCReg, en_bit); }
	void _enable(SAIEnableBit en_bit) { _set_bit(RCC->*SAIRCCReg, en_bit); }
	void _disable(SAIEnableBit en_bit) { _clear_bit(RCC->*SAIRCCReg, en_bit); }


#endif

#ifdef STM32H7
	//stm32h755xx.h:
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
		offset = offsetof(RCC_TypeDef, AHB4ENR),
	};
 
	template<typename T>
	__enable(T en_bit) { _set_bit_in_RCC_field(T::offset, en_bit); }

	__IO uint32_t RCCTypeDef::* GPIORCCReg = &RCC_TypeDef::AHB4ENR;
	bool _is_enabled(GPIOEnableBit en_bit) { return _read_bit(RCC->*GPIORCCReg, en_bit); }
	void _enable(GPIOEnableBit en_bit) { _set_bit(RCC->*GPIORCCReg, en_bit); }
	void _disable(GPIOEnableBit en_bit) { _clear_bit(RCC->*GPIORCCReg, en_bit); }

	enum class ADCEnableBit : uint32_t {
		_1 = RCC_AHB1ENR_ADC12EN,
		_2 = RCC_AHB1ENR_ADC12EN,
	};
	bool _is_enabled(ADCEnableBit en_bit) { return _read_bit(RCC->AHB1ENR, en_bit); }
	void _enable(ADCEnableBit en_bit) { _set_bit(RCC->AHB1ENR, en_bit); }
	void _disable(ADCEnableBit en_bit) { _clear_bit(RCC->AHB1ENR, en_bit); }

	enum class DMAEnableBit : uint32_t {
		_1 = RCC_AHB1ENR_DMA1EN,
		_2 = RCC_AHB1ENR_DMA2EN,
	};
	bool _is_enabled(DMAEnableBit en_bit) { return _read_bit(RCC->AHB1ENR, en_bit); }
	void _enable(DMAEnableBit en_bit) { _set_bit(RCC->AHB1ENR, en_bit); }
	void _disable(DMAEnableBit en_bit) { _clear_bit(RCC->AHB1ENR, en_bit); }

	enum class I2CEnableBit : uint32_t {
		_1 = RCC_APB1LENR_I2C1EN,
		_2 = RCC_APB1LENR_I2C2EN,
		_3 = RCC_APB1LENR_I2C3EN,
	};
	bool _is_enabled(I2CEnableBit en_bit) { return _read_bit(RCC->APB1LENR, en_bit); }
	void _enable(I2CEnableBit en_bit) { _set_bit(RCC->APB1LENR, en_bit); }
	void _disable(I2CEnableBit en_bit) { _clear_bit(RCC->APB1LENR, en_bit); }

	enum class SAIEnableBit : uint32_t {
		_1 = RCC_APB2ENR_SAI1EN,
		_2 = RCC_APB2ENR_SAI2EN,
		_3 = RCC_APB2ENR_SAI3EN,
	};
	__IO uint32_t RCCTypeDef::* SAIRCCReg = &RCC_TypeDef::APB2ENR;
	bool _is_enabled(SAIEnableBit en_bit) { return _read_bit(RCC->*SAIRCCReg, en_bit); }
	void _enable(SAIEnableBit en_bit) { _set_bit(RCC->*SAIRCCReg, en_bit); }
	void _disable(SAIEnableBit en_bit) { _clear_bit(RCC->*SAIRCCReg, en_bit); }

	enum class SAI4EnableBit : uint32_t {
		_4 = RCC_APB4ENR_SAI4EN,
	};
	__IO uint32_t RCCTypeDef::* SAI4RCCReg = &RCC_TypeDef::APB4ENR;
	bool _is_enabled(SAI4EnableBit en_bit) { return _read_bit(RCC->*SAI4RCCReg, en_bit); }
	void _enable(SAI4EnableBit en_bit) { _set_bit(RCC->*SAI4RCCReg, en_bit); }
	void _disable(SAI4EnableBit en_bit) { _clear_bit(RCC->*SAI4RCCReg, en_bit); }
#endif

#ifdef STM32MP1
#ifdef CORE_CM4

#endif
#endif

} //namespace mdrivlib
