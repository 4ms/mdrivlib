#pragma once
#include "drivers/pin.hh"
#include "drivers/stm32xx.h"
#include "drivers/uart_target.hh"
#include "stm32xx.h"

namespace mdrivlib
{

template<uint32_t BASE_ADDR>
class Uart {
	USART_TypeDef *const uart;

public:
	Uart()
		: uart{reinterpret_cast<USART_TypeDef *>(BASE_ADDR)} {
	}

	void init() {
		UartTarget::uart_init(BASE_ADDR);
	}

	void putchar(char c) {
		uart->TDR = c;
		UartTarget::delay_for_write(uart);
	}

	void write(const char *str) {
		while (*str) {
			uart->TDR = *str++;
			UartTarget::delay_for_write(uart);
		}
	}

	void write(uint32_t value) {
		if (!value) {
			write("0");
			return;
		}

		constexpr int MAX_DIGITS = 10;
		char buf[MAX_DIGITS + 1];
		int len = 0;
		do {
			const char digit = (char)(value % 10);
			buf[len++] = '0' + digit;
			value /= 10;
		} while (value && (len < MAX_DIGITS));
		buf[len] = '\0';

		for (int i = 0; i < len / 2; i++) {
			auto tmp = buf[i];
			buf[i] = buf[len - i - 1];
			buf[len - i - 1] = tmp;
		}

		write(buf);
	}
};

} // namespace mdrivlib
