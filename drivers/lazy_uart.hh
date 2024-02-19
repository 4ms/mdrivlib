#pragma once
#include "drivers/pin.hh"
#include "drivers/stm32xx.h"
#include "drivers/uart_conf.hh"
#include "drivers/uart_target.hh"
#include "stm32xx.h"

namespace mdrivlib
{
template<UartConf Conf>
class LazyUartPeriph {
public:
	LazyUartPeriph()
		: periph{reinterpret_cast<USART_TypeDef *const>(Conf.base_addr)} {
		init();
	}
	void init() {
		if constexpr (Conf.mode != UartConf::Mode::RXonly)
			Pin tx{Conf.TXPin, mdrivlib::PinMode::Alt};
		if constexpr (Conf.mode != UartConf::Mode::TXonly)
			Pin rx{Conf.RXPin, mdrivlib::PinMode::Alt};
		UartTarget<Conf>::uart_init();
	}

	USART_TypeDef *const periph;
};

template<UartConf Conf>
class LazyUart {

public:
	USART_TypeDef *const uart() {
		static auto _uart = LazyUartPeriph<Conf>{};
		return _uart.periph;
	}

	void init() {
		uart();
	}

	bool set_baudrate(uint32_t baudrate_in_hz) {
		return UartTarget<Conf>::set_baudrate(baudrate_in_hz);
	}

	void putchar(char c) {
		uart()->TDR = c;
		UartTarget<Conf>::delay_for_write(uart());
	}

	void write(const char *str) {
		while (*str) {
			uart()->TDR = *str++;
			UartTarget<Conf>::delay_for_write(uart());
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

	void transmit(uint8_t val) {
		uart()->TDR = val;
		UartTarget<Conf>::delay_for_write(uart());
	}

	bool receive(uint8_t *val) {
		if (UartTarget<Conf>::has_rx(uart())) {
			*val = uint8_t(uart()->RDR);
			return true;
		} else {
			return false;
		}
	}
};

} // namespace mdrivlib
