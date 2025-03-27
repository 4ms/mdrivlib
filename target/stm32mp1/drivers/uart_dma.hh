#pragma once
#include "drivers/dma_config_struct.hh"
#include "drivers/interrupt.hh"
#include "drivers/pin.hh"
#include "drivers/rcc.hh"
#include "drivers/stm32xx.h"
#include "drivers/uart_conf.hh"
#include "drivers/uart_target.hh"
#include "stm32mp1xx_ll_rcc.h"
#include "stm32mp1xx_ll_usart.h"
#include <atomic>
#include <cstdio>
#include <functional>
#include <span>

namespace mdrivlib
{

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle) {
	printf("TXC\n");
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle) {
	printf("RXC\n");
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle) {
	printf("ERR\n");
}

template<UartConf Conf>
void init_uart_clock() {
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
}

template<UartConf Conf>
IRQn get_uart_irqnum() {
	if constexpr (Conf.base_addr == USART1_BASE)
		return USART1_IRQn;
	if constexpr (Conf.base_addr == USART2_BASE)
		return USART2_IRQn;
	if constexpr (Conf.base_addr == USART3_BASE)
		return USART3_IRQn;
	if constexpr (Conf.base_addr == UART4_BASE)
		return UART4_IRQn;
	if constexpr (Conf.base_addr == UART5_BASE)
		return UART5_IRQn;
	if constexpr (Conf.base_addr == USART6_BASE)
		return USART6_IRQn;
	if constexpr (Conf.base_addr == UART7_BASE)
		return UART7_IRQn;
	if constexpr (Conf.base_addr == UART8_BASE)
		return UART8_IRQn;
}

template<UartConf Conf>
requires(Conf.base_addr == UART4_BASE || Conf.base_addr == UART5_BASE || Conf.base_addr == UART7_BASE ||
		 Conf.base_addr == UART8_BASE)
struct UartDma {

private:
	UART_HandleTypeDef hal_h{};
	DMA_HandleTypeDef dma_h{};
	static inline UartDma *_instance;
	std::atomic<bool> _is_busy = false;

	std::function<void(uint8_t)> _rx_callback;

public:
	void init() {
		init_uart_clock<Conf>();

		using enum UartConf::StopBits;
		using enum UartConf::Parity;
		using enum UartConf::Mode;

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

		IRQn irq_num = get_uart_irqnum<Conf>();
		IRQn dma_irq_num = {};

		if (Conf.base_addr == UART5_BASE) {
			// TODO: set this with the DMA config
			dma_irq_num = DMA2_Stream7_IRQn;
			dma_h.Instance = DMA2_Stream7;
			dma_h.Init.Request = DMA_REQUEST_UART5_TX;
		} else {
			//Not yet supported: TODO
			__BKPT(43);
		}
		dma_h.Init.Direction = DMA_MEMORY_TO_PERIPH;
		dma_h.Init.PeriphInc = DMA_PINC_DISABLE;
		dma_h.Init.MemInc = DMA_MINC_ENABLE;
		dma_h.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		dma_h.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		dma_h.Init.Mode = DMA_NORMAL;
		dma_h.Init.Priority = DMA_PRIORITY_LOW;
		dma_h.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		if (HAL_DMA_Init(&dma_h) != HAL_OK) {
			__BKPT(44);
		}

		dma_h.XferCpltCallback = dma_xfer_complete;

		__HAL_LINKDMA(&hal_h, hdmatx, dma_h);

		_instance = this;
		if (irq_num) {
			Interrupt::register_and_start_isr(irq_num, 0, 0, [this]() {
				HAL_UART_IRQHandler(&hal_h);
				if (_rx_callback) {
					_rx_callback(hal_h.Instance->RDR);
				}
			});
			Interrupt::register_and_start_isr(dma_irq_num, 0, 0, [this]() { HAL_DMA_IRQHandler(&dma_h); });
		}
	}

	bool is_busy() {
		return _is_busy;
	}

	void rx_callback(auto &&func) {
		_rx_callback = func;
	}

	// Data must be non-cacheable and remain in memory until xfer is complete
	bool send_dma(std::span<uint8_t> data) {
		while (_is_busy) {
			;
		}

		_is_busy = true;
		return HAL_UART_Transmit_DMA(&hal_h, data.data(), data.size()) == HAL_OK;
	}

	static void dma_xfer_complete(DMA_HandleTypeDef *dma) {
		printf("DMA XFER COMPLETE\n");
		volatile int x = 1;
		while (x)
			;
		_instance->_is_busy = false;
	}

	// static void delay_for_write(auto uart) {
	// 	while ((uart->ISR & USART_ISR_TXFE) == 0) //requires FIFO mode
	// 		;
	// }

	// static bool has_rx(auto uart) {
	// 	return (uart->ISR & USART_ISR_RXNE) != 0;
	// }

	// static bool set_baudrate(uint32_t baudrate_in_hz) {
	// 	return UartTarget<Conf>::set_baudrate(baudrate_in_hz);
	// }
};

template<UartConf Conf>
requires(Conf.base_addr == USART1_BASE || Conf.base_addr == USART2_BASE || Conf.base_addr == USART3_BASE ||
		 Conf.base_addr == USART6_BASE)
struct UsartDmaTarget {
	USART_HandleTypeDef hal_h{};
	DMA_HandleTypeDef dma_h{};

	void init() {
		init_uart_clock<Conf>();

		using enum UartConf::StopBits;
		using enum UartConf::Parity;
		using enum UartConf::Mode;

		hal_h.Instance = reinterpret_cast<USART_TypeDef *>(Conf.base_addr);
		hal_h.Init.BaudRate = Conf.baud;
		hal_h.Init.WordLength = Conf.wordlen == 8 ? USART_WORDLENGTH_8B
							  : Conf.wordlen == 7 ? USART_WORDLENGTH_7B
												  : USART_WORDLENGTH_9B;
		hal_h.Init.StopBits = Conf.stopbits == _1	? USART_STOPBITS_1
							: Conf.stopbits == _0_5 ? USART_STOPBITS_0_5
							: Conf.stopbits == _1_5 ? USART_STOPBITS_1_5
													: USART_STOPBITS_2;
		hal_h.Init.Parity = Conf.parity == None ? USART_PARITY_NONE
						  : Conf.parity == Odd	? USART_PARITY_ODD
												: USART_PARITY_EVEN;
		hal_h.Init.Mode = Conf.mode == TXRX ? USART_MODE_TX_RX : Conf.mode == TXonly ? USART_MODE_TX : USART_MODE_RX;
		hal_h.Init.CLKPolarity = USART_POLARITY_LOW;
		hal_h.Init.CLKPhase = USART_PHASE_1EDGE;
		hal_h.Init.CLKLastBit = USART_LASTBIT_DISABLE;
		hal_h.Init.ClockPrescaler = USART_PRESCALER_DIV1;

		hal_h.FifoMode = UART_FIFOMODE_ENABLE;

		auto err = HAL_USART_Init(&hal_h);
		if (err != HAL_OK) {
			// __BKPT(43);
		}

		// disable peripheral and wait for completion
		hal_h.Instance->CR1 &= ~USART_CR1_UE;
		while (hal_h.Instance->CR1 & USART_CR1_UE)
			;

		// HAL_USART_Init() enables synchronous mode by default
		// this triggers RX for every TX
		hal_h.Instance->CR2 &= ~USART_CR2_CLKEN;

		//HAL_USART_Init() does not enable Fifo Mode, so we must do it manually
		hal_h.Instance->CR1 |= USART_CR1_FIFOEN;

		// enable peripheral
		hal_h.Instance->CR1 |= USART_CR1_UE;
	}

	static void delay_for_write(auto uart) {
		while ((uart->ISR & USART_ISR_TXFE) == 0) //requires FIFO mode
			;
	}

	static bool has_rx(auto uart) {
		return (uart->ISR & USART_ISR_RXNE) != 0;
	}

	static bool set_baudrate(uint32_t baudrate_in_hz) {
		return UartTarget<Conf>::set_baudrate(baudrate_in_hz);
	}
};
} // namespace mdrivlib
