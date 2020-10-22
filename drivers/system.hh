#pragma once

#include "stm32f7xx_ll_bus.h"
#include "stm32xx.h"

// Todo: refactor for LL intead of HAL
struct System {
	System() {}

	static void SetVectorTable(uint32_t reset_address)
	{
		SCB->VTOR = reset_address & (uint32_t)0x1FFFFF80;
	}

	static void init_clocks(const RCC_OscInitTypeDef &osc_def,
							const RCC_ClkInitTypeDef &clk_def,
							const RCC_PeriphCLKInitTypeDef &pclk_def,
							const uint32_t systick_freq_hz = 1000)
	{
		__HAL_RCC_PWR_CLK_ENABLE();
		__HAL_RCC_SYSCFG_CLK_ENABLE();

		RCC_OscInitTypeDef osc_def_ = osc_def;
		HAL_RCC_OscConfig(&osc_def_);

		HAL_PWREx_EnableOverDrive();

		RCC_ClkInitTypeDef clk_def_ = clk_def;
		HAL_RCC_ClockConfig(&clk_def_, FLASH_LATENCY_7);

		RCC_PeriphCLKInitTypeDef pclk_def_ = pclk_def;
		HAL_RCCEx_PeriphCLKConfig(&pclk_def_);

		HAL_RCC_EnableCSS();

		HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / systick_freq_hz);
		HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
	}

	static uint32_t encode_nvic_priority(uint32_t pri1, uint32_t pri2)
	{
		return NVIC_EncodePriority(NVIC_GetPriorityGrouping(), pri1, pri2);
	}

	static constexpr void enable_gpio_rcc(GPIO_TypeDef *port)
	{
		if (port == nullptr)
			return;
#ifdef GPIOA
		else if (port == GPIOA && !READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN))
			LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
#endif
#ifdef GPIOB
		else if (port == GPIOB && !READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN))
			LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
#endif
#ifdef GPIOC
		else if (port == GPIOC && !READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN))
			LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
#endif
#ifdef GPIOD
		else if (port == GPIOD && !READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN))
			LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
#endif
#ifdef GPIOE
		else if (port == GPIOE && !READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOEEN))
			LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);
#endif
#ifdef GPIOF
		else if (port == GPIOF && !READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOFEN))
			LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);
#endif
#ifdef GPIOG
		else if (port == GPIOG && !READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOGEN))
			LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOG);
#endif
#ifdef GPIOH
		else if (port == GPIOH && !READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOHEN))
			LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOH);
#endif
#ifdef GPIOI
		else if (port == GPIOI && !READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOIEN))
			LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOI);
#endif
#ifdef GPIOJ
		else if (port == GPIOJ && !READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOJEN))
			LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOJ);
#endif
#ifdef GPIOK
		else if (port == GPIOK && !READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOKEN))
			LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOK);
#endif
	}

	static constexpr void enable_adc_rcc(ADC_TypeDef *ADCx)
	{
		if (ADCx == nullptr)
			return;
#ifdef ADC1
		if (ADCx == ADC1)
			LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);
#endif
#ifdef ADC2
		else if (ADCx == ADC2)
			LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC2);
#endif
#ifdef ADC3
		else if (ADCx == ADC3)
			LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC3);
#endif
	}

	static constexpr void enable_dma_rcc(const DMA_TypeDef *DMAx)
	{
		if (DMAx == nullptr)
			return;
#ifdef DMA1
		else if (DMAx == DMA1)
			LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
#endif
#ifdef DMA2
		else if (DMAx == DMA2)
			LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);
#endif
#ifdef DMA3
		else if (DMAx == DMA3)
			LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA3);
#endif
	}

	static constexpr void enable_i2c_rcc(I2C_TypeDef *I2Cx)
	{
		if (I2Cx == nullptr)
			return;
#ifdef I2C1
		else if (I2Cx == I2C1)
			LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);
#endif
#ifdef I2C2
		else if (I2Cx == I2C2)
			LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);
#endif
#ifdef I2C3
		else if (I2Cx == I2C3)
			LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C3);
#endif
	}

	static constexpr void disable_i2c_rcc(I2C_TypeDef *I2Cx)
	{
		if (I2Cx == nullptr)
			return;
#ifdef I2C1
		else if (I2Cx == I2C1)
			LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I2C1);
#endif
#ifdef I2C2
		else if (I2Cx == I2C2)
			LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I2C2);
#endif
#ifdef I2C3
		else if (I2Cx == I2C3)
			LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I2C3);
#endif
	}

	static constexpr void enable_sai_rcc(SAI_TypeDef *SAIx)
	{
		if (SAIx == nullptr)
			return;
#ifdef SAI1
		else if (SAIx == SAI1)
			LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SAI1);
#endif
#ifdef SAI2
		else if (SAIx == SAI2)
			LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SAI2);
#endif
#ifdef SAI3
		else if (SAIx == SAI3)
			LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SAI3);
#endif
#ifdef SAI4
		else if (SAIx == SAI4)
			LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_SAI4);
#endif
	}

	static constexpr void disable_sai_rcc(SAI_TypeDef *SAIx)
	{
		if (SAIx == nullptr)
			return;
#ifdef SAI1
		else if (SAIx == SAI1)
			LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_SAI1);
#endif
#ifdef SAI2
		else if (SAIx == SAI2)
			LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_SAI2);
#endif
#ifdef SAI3
		else if (SAIx == SAI3)
			LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_SAI3);
#endif
#ifdef SAI4
		else if (SAIx == SAI4)
			LL_APB4_GRP1_DisableClock(LL_APB4_GRP1_PERIPH_SAI4);
#endif
	}

	static void enable_tim_rcc(TIM_TypeDef *TIM)
	{
#ifdef TIM1
		if (TIM == TIM1)
			LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
#endif
#ifdef TIM2
		if (TIM == TIM2)
			LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
#endif
#ifdef TIM3
		if (TIM == TIM3)
			LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
#endif
#ifdef TIM4
		if (TIM == TIM4)
			LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);
#endif
#ifdef TIM5
		if (TIM == TIM5)
			LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM5);
#endif
#ifdef TIM6
		if (TIM == TIM6)
			LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);
#endif
#ifdef TIM7
		if (TIM == TIM7)
			LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM7);
#endif
#ifdef TIM8
		if (TIM == TIM8)
			LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM8);
#endif
#ifdef TIM9
		if (TIM == TIM9)
			LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM9);
#endif
#ifdef TIM10
		if (TIM == TIM10)
			LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM10);
#endif
#ifdef TIM11
		if (TIM == TIM11)
			LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM11);
#endif
#ifdef TIM12
		if (TIM == TIM12)
			LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM12);
#endif
#ifdef TIM13
		if (TIM == TIM13)
			LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM13);
#endif
#ifdef TIM14
		if (TIM == TIM14)
			LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM14);
#endif
#ifdef TIM15
		if (TIM == TIM15)
			LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM15);
#endif
#ifdef TIM16
		if (TIM == TIM16)
			LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM16);
#endif
#ifdef TIM17
		if (TIM == TIM17)
			LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM17);
#endif
	}

	static inline constexpr uint8_t kTimPeriphMax = 17;

	static uint8_t tim_periph_to_num(TIM_TypeDef *TIM)
	{
		if (TIM == nullptr)
			return 0;
#ifdef TIM1
		else if (TIM == TIM1)
			return 1;
#endif
#ifdef TIM2
		else if (TIM == TIM2)
			return 2;
#endif
#ifdef TIM3
		else if (TIM == TIM3)
			return 3;
#endif
#ifdef TIM4
		else if (TIM == TIM4)
			return 4;
#endif
#ifdef TIM5
		else if (TIM == TIM5)
			return 5;
#endif
#ifdef TIM6
		else if (TIM == TIM6)
			return 6;
#endif
#ifdef TIM7
		else if (TIM == TIM7)
			return 7;
#endif
#ifdef TIM8
		else if (TIM == TIM8)
			return 8;
#endif
#ifdef TIM9
		else if (TIM == TIM9)
			return 9;
#endif
#ifdef TIM10
		else if (TIM == TIM10)
			return 10;
#endif
#ifdef TIM11
		else if (TIM == TIM11)
			return 11;
#endif
#ifdef TIM12
		else if (TIM == TIM12)
			return 12;
#endif
#ifdef TIM13
		else if (TIM == TIM13)
			return 13;
#endif
#ifdef TIM14
		else if (TIM == TIM14)
			return 14;
#endif
#ifdef TIM15
		else if (TIM == TIM15)
			return 15;
#endif
#ifdef TIM16
		else if (TIM == TIM16)
			return 16;
#endif
#ifdef TIM17
		else if (TIM == TIM17)
			return 17;
#endif
		else
			return 0;
	}
	static IRQn_Type tim_periph_to_IRQn(TIM_TypeDef *TIM)
	{
		if (TIM == nullptr)
			return (IRQn_Type)(0);
#ifdef TIM1
		else if (TIM == TIM1)
			return TIM1_UP_TIM10_IRQn;
#endif
#ifdef TIM2
		else if (TIM == TIM2)
			return TIM2_IRQn;
#endif
#ifdef TIM3
		else if (TIM == TIM3)
			return TIM3_IRQn;
#endif
#ifdef TIM4
		else if (TIM == TIM4)
			return TIM4_IRQn;
#endif
#ifdef TIM5
		else if (TIM == TIM5)
			return TIM5_IRQn;
#endif
#ifdef TIM6
		else if (TIM == TIM6)
			return TIM6_DAC_IRQn;
#endif
#ifdef TIM7
		else if (TIM == TIM7)
			return TIM7_IRQn;
#endif
#ifdef TIM8
		else if (TIM == TIM8)
			return TIM8_UP_TIM13_IRQn;
#endif
#ifdef TIM9
		else if (TIM == TIM9)
			return TIM1_BRK_TIM9_IRQn;
#endif
#ifdef TIM10
		else if (TIM == TIM10)
			return TIM1_UP_TIM10_IRQn;
#endif
#ifdef TIM11
		else if (TIM == TIM11)
			return TIM1_TRG_COM_TIM11_IRQn;
#endif
#ifdef TIM12
		else if (TIM == TIM12)
			return TIM8_BRK_TIM12_IRQn;
#endif
#ifdef TIM13
		else if (TIM == TIM13)
			return TIM8_UP_TIM13_IRQn;
#endif
#ifdef TIM14
		else if (TIM == TIM14)
			return TIM8_TRG_COM_TIM14_IRQn;
#endif
#ifdef TIM15
		else if (TIM == TIM15)
			return TIM15_IRQn;
#endif
#ifdef TIM16
		else if (TIM == TIM16)
			return TIM16_IRQn;
#endif
#ifdef TIM17
		else if (TIM == TIM17)
			return TIM17_IRQn;
#endif
		else
			return (IRQn_Type)(0);
	}

	static uint32_t tim_periph_max_freq(TIM_TypeDef *TIM)
	{
		// APB2 --> divider = 1;
		// APB1 --> divider = 2;
		uint32_t divider;
		auto TIMx_BASE_ADDR = reinterpret_cast<uintptr_t>(TIM);
		if (TIMx_BASE_ADDR >= APB1PERIPH_BASE && TIMx_BASE_ADDR < APB2PERIPH_BASE)
			divider = 2;
		else if (TIMx_BASE_ADDR >= APB2PERIPH_BASE)
			divider = 1;
		else
			divider = 1; // unknown, error?
		return HAL_RCC_GetHCLKFreq() / divider;
	}
};

