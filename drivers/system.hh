#pragma once
#include "arch.hh"
#include "periph.hh"
#include "rcc.hh"
#include "stm32xx.h"

// Todo: refactor to use CMSIS header intead of HAL (RCC_OscInitTypeDef, etc)
// Todo: move rcc enable/disable to RCC_Control
struct System {
	System() {
	}

	static void SetVectorTable(uint32_t reset_address);

	static void init_clocks(const RCC_OscInitTypeDef &osc_def,
							const RCC_ClkInitTypeDef &clk_def,
							const RCC_PeriphCLKInitTypeDef &pclk_def,
							const uint32_t systick_freq_hz = 1000);

	static uint32_t encode_nvic_priority(uint32_t pri1, uint32_t pri2);

	struct ADC {
		static void enable(unsigned periph_num) {
			if (periph_num == 1)
				RCC_Control::ADC_1::set();
			else if (periph_num == 2)
				RCC_Control::ADC_2::set();
			else if (periph_num == 3)
				RCC_Control::ADC_3::set();
		}
		static void enable(ADC_TypeDef *ADCx) {
			enable(peripherals::ADC::to_num(ADCx));
		}
		static void disable(unsigned periph_num) {
			if (periph_num == 1)
				RCC_Control::ADC_1::clear();
			else if (periph_num == 2)
				RCC_Control::ADC_2::clear();
			else if (periph_num == 3)
				RCC_Control::ADC_3::clear();
		}
		static void disable(ADC_TypeDef *ADCx) {
			disable(peripherals::ADC::to_num(ADCx));
		}
	};

	struct DMA {
		static void enable(const DMA_TypeDef *DMAx) {
			if (DMAx == nullptr)
				return;
			else if (DMAx == DMA1)
				RCC_Control::DMA_1::set();
			else if (DMAx == DMA2)
				RCC_Control::DMA_2::set();
			else if (DMAx == reinterpret_cast<DMA_TypeDef *>(BDMA))
				RCC_Control::BDMA_P::set();
		}
		static void disable(const DMA_TypeDef *DMAx) {
			if (DMAx == nullptr)
				return;
			else if (DMAx == DMA1)
				RCC_Control::DMA_1::clear();
			else if (DMAx == DMA2)
				RCC_Control::DMA_2::clear();
			else if (DMAx == reinterpret_cast<DMA_TypeDef *>(BDMA))
				RCC_Control::BDMA_P::clear();
		}
	};

	struct I2C {
		static void enable(I2C_TypeDef *I2Cx) {
			if (I2Cx == nullptr)
				return;
			else if (I2Cx == I2C1)
				RCC_Control::I2C_1::set();
			else if (I2Cx == I2C2)
				RCC_Control::I2C_2::set();
			else if (I2Cx == I2C3)
				RCC_Control::I2C_3::set();
		}
		static void disable(I2C_TypeDef *I2Cx) {
			if (I2Cx == nullptr)
				return;
			else if (I2Cx == I2C1)
				RCC_Control::I2C_1::clear();
			else if (I2Cx == I2C2)
				RCC_Control::I2C_2::clear();
			else if (I2Cx == I2C3)
				RCC_Control::I2C_3::clear();
		}
	};

	struct SAI {
		static void enable(SAI_TypeDef *SAIx) {
			if (SAIx == nullptr)
				return;
			else if (SAIx == SAI1)
				RCC_Control::SAI_1::set();
			else if (SAIx == SAI2)
				RCC_Control::SAI_2::set();
			else if (SAIx == SAI3)
				RCC_Control::SAI_3::set();
			else if (SAIx == SAI4)
				RCC_Control::SAI_4::set();
		}
		static void disable(SAI_TypeDef *SAIx) {
			if (SAIx == nullptr)
				return;
			else if (SAIx == SAI1)
				RCC_Control::SAI_1::clear();
			else if (SAIx == SAI2)
				RCC_Control::SAI_2::clear();
			else if (SAIx == SAI3)
				RCC_Control::SAI_3::clear();
			else if (SAIx == SAI4)
				RCC_Control::SAI_4::clear();
		}
	};

	struct TIM {
		static void enable(unsigned periph_num) {
			if (periph_num < 1 || periph_num > peripherals::TIM::NumPeriph)
				return;
			else if (periph_num == 1)
				RCC_Control::TIM_1::set();
			else if (periph_num == 2)
				RCC_Control::TIM_2::set();
			else if (periph_num == 3)
				RCC_Control::TIM_3::set();
			else if (periph_num == 4)
				RCC_Control::TIM_4::set();
			else if (periph_num == 5)
				RCC_Control::TIM_5::set();
			else if (periph_num == 6)
				RCC_Control::TIM_6::set();
			else if (periph_num == 7)
				RCC_Control::TIM_7::set();
			else if (periph_num == 8)
				RCC_Control::TIM_8::set();
			else if (periph_num == 9)
				RCC_Control::TIM_9::set();
			else if (periph_num == 10)
				RCC_Control::TIM_10::set();
			else if (periph_num == 11)
				RCC_Control::TIM_11::set();
			else if (periph_num == 12)
				RCC_Control::TIM_12::set();
			else if (periph_num == 13)
				RCC_Control::TIM_13::set();
			else if (periph_num == 14)
				RCC_Control::TIM_14::set();
			else if (periph_num == 15)
				RCC_Control::TIM_15::set();
			else if (periph_num == 16)
				RCC_Control::TIM_16::set();
			else if (periph_num == 17)
				RCC_Control::TIM_17::set();
		}
		static void disable(unsigned periph_num) {
			if (periph_num < 1 || periph_num > peripherals::TIM::NumPeriph)
				return;
			else if (periph_num == 1)
				RCC_Control::TIM_1::clear();
			else if (periph_num == 2)
				RCC_Control::TIM_2::clear();
			else if (periph_num == 3)
				RCC_Control::TIM_3::clear();
			else if (periph_num == 4)
				RCC_Control::TIM_4::clear();
			else if (periph_num == 5)
				RCC_Control::TIM_5::clear();
			else if (periph_num == 6)
				RCC_Control::TIM_6::clear();
			else if (periph_num == 7)
				RCC_Control::TIM_7::clear();
			else if (periph_num == 8)
				RCC_Control::TIM_8::clear();
			else if (periph_num == 9)
				RCC_Control::TIM_9::clear();
			else if (periph_num == 10)
				RCC_Control::TIM_10::clear();
			else if (periph_num == 11)
				RCC_Control::TIM_11::clear();
			else if (periph_num == 12)
				RCC_Control::TIM_12::clear();
			else if (periph_num == 13)
				RCC_Control::TIM_13::clear();
			else if (periph_num == 14)
				RCC_Control::TIM_14::clear();
			else if (periph_num == 15)
				RCC_Control::TIM_15::clear();
			else if (periph_num == 16)
				RCC_Control::TIM_16::clear();
			else if (periph_num == 17)
				RCC_Control::TIM_17::clear();
		}
		static void enable(TIM_TypeDef *TIMx) {
			enable(peripherals::TIM::to_num(TIMx));
		}
		static void disable(TIM_TypeDef *TIMx) {
			disable(peripherals::TIM::to_num(TIMx));
		}
	};
};

