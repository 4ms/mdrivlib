#include "drivers/pin.hh"
#include "drivers/rcc.hh"
#include "drivers/stm32xx.h"

namespace mdrivlib
{
//FIXME: use ConfT
struct UartTarget {
	static void uart_init(uint32_t BASE_ADDR) {
		//TODO: proper conf to setup any UART, including pins, baud rate, etc
		if (BASE_ADDR == USART1_BASE)
			RCC_Enable::USART1_::set();
		if (BASE_ADDR == USART2_BASE)
			RCC_Enable::USART2_::set();
		if (BASE_ADDR == USART3_BASE)
			RCC_Enable::USART3_::set();
		if (BASE_ADDR == UART4_BASE)
			RCC_Enable::UART4_::set();
		if (BASE_ADDR == UART5_BASE)
			RCC_Enable::UART5_::set();
		if (BASE_ADDR == USART6_BASE)
			RCC_Enable::USART6_::set();
		if (BASE_ADDR == UART7_BASE)
			RCC_Enable::UART7_::set();
		if (BASE_ADDR == UART8_BASE)
			RCC_Enable::UART8_::set();

		if (BASE_ADDR == UART4_BASE || BASE_ADDR == UART5_BASE || BASE_ADDR == UART7_BASE || BASE_ADDR == UART8_BASE) {
			UART_HandleTypeDef hal_h;
			hal_h.Instance = reinterpret_cast<USART_TypeDef *>(BASE_ADDR);
			hal_h.Init.BaudRate = 115200;
			hal_h.Init.WordLength = UART_WORDLENGTH_8B;
			hal_h.Init.StopBits = UART_STOPBITS_1;
			hal_h.Init.Parity = UART_PARITY_NONE;
			hal_h.Init.Mode = UART_MODE_TX_RX;
			hal_h.Init.HwFlowCtl = UART_HWCONTROL_NONE;
			hal_h.Init.OverSampling = UART_OVERSAMPLING_16;
			hal_h.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
			auto err = HAL_UART_Init(&hal_h);
			if (err != HAL_OK) {
				__BKPT(43);
			}
		} else {
			// USART_HandleTypeDef hal_h;
			// hal_h.Instance = reinterpret_cast<USART_TypeDef *>(BASE_ADDR);
			// hal_h.Init.BaudRate = 115200;
			// hal_h.Init.WordLength = USART_WORDLENGTH_8B;
			// hal_h.Init.StopBits = USART_STOPBITS_1;
			// hal_h.Init.Parity = USART_PARITY_NONE;
			// hal_h.Init.Mode = USART_MODE_TX_RX;
			// hal_h.Init.CLKPolarity = USART_POLARITY_LOW;
			// hal_h.Init.CLKPhase = USART_PHASE_1EDGE;
			// hal_h.Init.CLKLastBit = USART_LASTBIT_DISABLE;
			// auto err = HAL_USART_Init(&hal_h);
			// if (err != HAL_OK) {
			// 	__BKPT(43);
			// }
		}
	}

	static void delay_for_write(auto uart) {
		while ((uart->ISR & USART_ISR_TXE) == 0)
			;
	}
};
} // namespace mdrivlib
