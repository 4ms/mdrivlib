#include "drivers/pin.hh"
#include "drivers/stm32xx.h"

namespace mdrivlib
{
struct UartTarget {
	static void uart_init(uint32_t BASE_ADDR) {
		//TODO: proper conf to setup any UART
		if (BASE_ADDR == UART4_BASE) {
			__HAL_RCC_UART4_CLK_ENABLE();
			Pin tx{GPIO::G, 11, PinMode::Alt, LL_GPIO_AF_6, PinPull::None, PinPolarity::Normal, PinSpeed::VeryHigh};
			Pin rx{GPIO::B, 2, PinMode::Alt, LL_GPIO_AF_8, PinPull::None, PinPolarity::Normal, PinSpeed::VeryHigh};
		} else if (BASE_ADDR == USART6_BASE) {
			__HAL_RCC_USART6_CLK_ENABLE();
			Pin tx{GPIO::C, 6, PinMode::Alt, LL_GPIO_AF_7, PinPull::None, PinPolarity::Normal, PinSpeed::VeryHigh};
			Pin rx{GPIO::C, 7, PinMode::Alt, LL_GPIO_AF_7, PinPull::None, PinPolarity::Normal, PinSpeed::VeryHigh};
		} else
			__BKPT(42);

		UART_HandleTypeDef hal_h;
		hal_h.Instance = uart;
		hal_h.Init.BaudRate = 115200;
		hal_h.Init.ClockPrescaler = UART_PRESCALER_DIV1;
		hal_h.Init.WordLength = UART_WORDLENGTH_8B;
		hal_h.Init.StopBits = UART_STOPBITS_1;
		hal_h.Init.Parity = UART_PARITY_NONE;
		hal_h.Init.Mode = UART_MODE_TX_RX;
		hal_h.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		hal_h.Init.OverSampling = UART_OVERSAMPLING_16;
		hal_h.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
		hal_h.FifoMode = UART_FIFOMODE_ENABLE;
		hal_h.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
		auto err = HAL_UART_Init(&hal_h);
		if (err != HAL_OK) {
			__BKPT(43);
		}
	}

	static void delay_for_write(auto uart) {
		while ((uart->ISR & USART_ISR_TXFT) == 0)
			;
	}
};
} // namespace mdrivlib
