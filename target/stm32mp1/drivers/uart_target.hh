#pragma once
#include "drivers/pin.hh"
#include "drivers/rcc.hh"
#include "drivers/stm32xx.h"
#include "uart_conf.hh"

namespace mdrivlib
{
template<UartConf Conf>
struct UartTarget {
	static void uart_init() {
		constexpr bool is_usart = (Conf.base_addr == USART1_BASE || Conf.base_addr == USART2_BASE ||
								   Conf.base_addr == USART3_BASE || Conf.base_addr == USART6_BASE);
		if constexpr (Conf.base_addr == USART1_BASE)
			RCC_Enable::USART1_::set();
		if constexpr (Conf.base_addr == USART2_BASE)
			RCC_Enable::USART2_::set();
		if constexpr (Conf.base_addr == USART3_BASE)
			RCC_Enable::USART3_::set();
		if constexpr (Conf.base_addr == UART4_BASE)
			RCC_Enable::UART4_::set();
		if constexpr (Conf.base_addr == UART5_BASE)
			RCC_Enable::UART5_::set();
		if constexpr (Conf.base_addr == USART6_BASE)
			RCC_Enable::USART6_::set();
		if constexpr (Conf.base_addr == UART7_BASE)
			RCC_Enable::UART7_::set();
		if constexpr (Conf.base_addr == UART8_BASE)
			RCC_Enable::UART8_::set();

		using enum UartConf::StopBits;
		using enum UartConf::Parity;
		using enum UartConf::Mode;

		if constexpr (is_usart) {
			UART_HandleTypeDef hal_h;
			hal_h.Instance = reinterpret_cast<USART_TypeDef *>(Conf.base_addr);
			hal_h.Init.BaudRate = Conf.baud;
			hal_h.Init.WordLength = Conf.wordlen == 8 ? UART_WORDLENGTH_8B
								  : Conf.wordlen == 7 ? UART_WORDLENGTH_7B
													  : UART_WORDLENGTH_9B;
			hal_h.Init.StopBits = Conf.stopbits == _1	? UART_STOPBITS_1
								: Conf.stopbits == _0_5 ? UART_STOPBITS_0_5
								: Conf.stopbits == _1_5 ? UART_STOPBITS_1_5
														: UART_STOPBITS_2;
			hal_h.Init.Parity = Conf.parity == None ? UART_PARITY_NONE
							  : Conf.parity == Odd	? UART_PARITY_ODD
													: UART_PARITY_EVEN;
			hal_h.Init.Mode = Conf.mode == TXRX ? UART_MODE_TX_RX : Conf.mode == TXonly ? UART_MODE_TX : UART_MODE_RX;
			hal_h.Init.HwFlowCtl = UART_HWCONTROL_NONE;
			hal_h.Init.OverSampling = UART_OVERSAMPLING_16;
			hal_h.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
			hal_h.Init.ClockPrescaler = UART_PRESCALER_DIV1;

			hal_h.FifoMode = UART_FIFOMODE_ENABLE;
			hal_h.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
			auto err = HAL_UART_Init(&hal_h);
			if (err != HAL_OK) {
				// __BKPT(43);
			}
		} else {
			// USART_HandleTypeDef hal_h;
			// hal_h.Instance = reinterpret_cast<USART_TypeDef *>(Conf.base_addr);
			// hal_h.Init.BaudRate = Conf.baud;
			// hal_h.Init.WordLength = Conf.wordlen == 8 ? USART_WORDLENGTH_8B
			// 					  : Conf.wordlen == 7 ? USART_WORDLENGTH_7B
			// 										  : USART_WORDLENGTH_9B;
			// hal_h.Init.StopBits = Conf.stopbits == _1	? USART_STOPBITS_1
			// 					: Conf.stopbits == _0_5 ? USART_STOPBITS_0_5
			// 					: Conf.stopbits == _1_5 ? USART_STOPBITS_1_5
			// 											: USART_STOPBITS_2;
			// hal_h.Init.Parity = Conf.parity == None ? USART_PARITY_NONE
			// 				  : Conf.parity == Odd	? USART_PARITY_ODD
			// 										: USART_PARITY_EVEN;
			// hal_h.Init.Mode = Conf.mode == TXRX	  ? USART_MODE_TX_RX
			// 				: Conf.mode == TXonly ? USART_MODE_TX
			// 									  : USART_MODE_RX;
			// hal_h.Init.CLKPolarity = USART_POLARITY_LOW;
			// hal_h.Init.CLKPhase = USART_PHASE_1EDGE;
			// hal_h.Init.CLKLastBit = USART_LASTBIT_DISABLE;
			// hal_h.Init.ClockPrescaler = USART_PRESCALER_DIV1;
			// auto err = HAL_USART_Init(&hal_h);
			// if (err != HAL_OK) {
			// 	// __BKPT(43);
			// }
		}
	}

	static void delay_for_write(auto uart) {
		while ((uart->ISR & USART_ISR_TXFT) == 0)
			;
	}
};
} // namespace mdrivlib
