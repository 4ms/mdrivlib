#pragma once
#include "drivers/rcc.hh"
#include "drivers/rcc_xbar.hh"
#include "drivers/stm32xx.h"
#include "drivers/uart_conf.hh"

// UART/USART target for STM32MP2.
//
// Register-level (no HAL): the MP2 HAL computes baud rates from the RCC flexgen
// tree, which this driver avoids by clocking every UART from HSI (64 MHz) via
// its flexgen XBAR channel. The baud rate then does not depend on any PLL that
// TF-A may or may not have configured. Same approach as the verified
// stm32mp2-baremetal shared/print/uart_print.c.
//
// Note: flexgen channels are shared by UART pairs (UART2/4, UART3/5, UART7/8),
// so both members of a pair always run from the same kernel clock.

namespace mdrivlib
{
template<UartConf Conf>
struct UartTarget {
	static constexpr uint32_t KernelClockHz = 64'000'000; // HSI

	// Flexgen XBAR channel (RM0457 RCC kernel clock distribution, MP25x default mapping)
	static constexpr unsigned flexgen_channel() {
		if constexpr (Conf.base_addr == USART1_BASE)
			return 19;
		else if constexpr (Conf.base_addr == USART2_BASE || Conf.base_addr == UART4_BASE)
			return 8;
		else if constexpr (Conf.base_addr == USART3_BASE || Conf.base_addr == UART5_BASE)
			return 9;
		else if constexpr (Conf.base_addr == USART6_BASE)
			return 20;
		else if constexpr (Conf.base_addr == UART7_BASE || Conf.base_addr == UART8_BASE)
			return 21;
		else if constexpr (Conf.base_addr == UART9_BASE)
			return 22;
		else
			static_assert(false, "Unknown UART base address");
	}

	static void enable_rcc() {
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
		if constexpr (Conf.base_addr == UART9_BASE)
			RCC_Enable::UART9_::set();
	}

	static void uart_init() {
		// Kernel clock = HSI / 1
		FlexbarConf{.PLL = FlexbarConf::PLLx::HSI, .findiv = 0, .prediv = 0}.init(
			flexgen_channel());

		enable_rcc();

		auto uart = reinterpret_cast<USART_TypeDef *>(Conf.base_addr);

		uart->CR1 = 0;
		while (uart->CR1 & USART_CR1_UE)
			;

		using enum UartConf::StopBits;
		using enum UartConf::Parity;
		using enum UartConf::Mode;

		// Word length M[1:0]: 00 = 8 bits, 01 = 9 bits, 10 = 7 bits. Parity takes one of these bits.
		const uint32_t total_bits = Conf.wordlen + (Conf.parity == None ? 0 : 1);
		const uint32_t m_bits = total_bits == 9 ? USART_CR1_M0 : total_bits == 7 ? USART_CR1_M1 : 0;

		const uint32_t parity_bits = Conf.parity == None ? 0 : Conf.parity == Odd ? (USART_CR1_PCE | USART_CR1_PS) : USART_CR1_PCE;

		const uint32_t mode_bits = Conf.mode == TXRX	 ? (USART_CR1_TE | USART_CR1_RE)
								 : Conf.mode == TXonly ? USART_CR1_TE
													   : USART_CR1_RE;

		const uint32_t stop_bits = Conf.stopbits == _1	 ? 0
								 : Conf.stopbits == _0_5 ? USART_CR2_STOP_0
								 : Conf.stopbits == _2	 ? USART_CR2_STOP_1
														 : (USART_CR2_STOP_0 | USART_CR2_STOP_1);

		uart->CR2 = stop_bits;
		uart->CR3 = 0;
		uart->PRESC = 0;
		uart->BRR = brr(Conf.baud);

		uart->CR1 = USART_CR1_FIFOEN | m_bits | parity_bits | mode_bits;
		uart->CR1 |= USART_CR1_UE;
	}

	static constexpr uint32_t brr(uint32_t baud) {
		// Oversampling by 16
		return (KernelClockHz + baud / 2) / baud;
	}

	static void delay_for_write(auto uart) {
		while ((uart->ISR & USART_ISR_TXFE) == 0) //requires FIFO mode
			;
	}

	static bool has_rx(auto uart) {
		return (uart->ISR & USART_ISR_RXNE_RXFNE) != 0;
	}

	static bool set_baudrate(uint32_t baudrate_in_hz) {
		auto uart = reinterpret_cast<USART_TypeDef *>(Conf.base_addr);

		uart->CR1 &= ~USART_CR1_UE;
		while (uart->CR1 & USART_CR1_UE)
			;
		uart->BRR = brr(baudrate_in_hz);
		uart->CR1 |= USART_CR1_UE;

		return true;
	}
};
} // namespace mdrivlib
