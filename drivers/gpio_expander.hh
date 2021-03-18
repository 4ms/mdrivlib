#pragma once
#include "i2c.hh"
#include "interrupt.hh"
#include "pin.hh"
#include <cstdint>

namespace mdrivlib
{
struct GPIO_expander_conf {
	uint32_t addr;
	Pin int_pin;
	IRQType IRQn;
	enum PinDirection { Output = 0, Input = 1 };
	PinDirection config[16];
};

struct GPIOExpander {
	static constexpr uint8_t NumChans = 16;

	GPIOExpander(I2CPeriph &i2c, const GPIO_expander_conf &conf)
		: _conf{conf}
		, _device_addr{conf.addr << 1}
		, _i2c{i2c} {
	}

	void init() {
		_i2c.write(_conf.addr );
	}
	void start_ISR() {
		// Create an EXTI interrupt to respond to chip telling us there's a pin change
		// InterruptManager::registerISR(_conf.IRQn, [this]() { read_pins(); });
	}
	void read_pins() {
		// write InputPort0 or 1
		// read InputPort 0 or 1
	}

	void set_output_values(uint8_t port, uint8_t mask) {
		// write OutputPort 0 or 1 with mask
	}

	uint16_t collect_last_reading() {
		uint16_t clear_mask = _cur_port ? 0x00FF : 0xFF00;
		last_reading &= clear_mask;
		last_reading |= _data[0] << (_cur_port * 8);
		return last_reading;
	}

	bool get_val(unsigned chan) {
		if (chan >= NumChans)
			return false;
		return last_reading & (1 << chan) ? true : false;
	}

private:
	const GPIO_expander_conf &_conf;
	const uint8_t _device_addr;
	I2CPeriph &_i2c;
	uint16_t last_reading;
	uint8_t _data[2];
	unsigned _cur_port = 0;

	enum Registers : uint8_t {
		InputPort0 = 0x00,
		InputPort1 = 0x01,
		OutputPort0 = 0x02,
		OutputPort1 = 0x03,
		PolarityPort0 = 0x04,
		PolarityPort1 = 0x05,
		ConfigPort0 = 0x06,
		ConfigPort1 = 0x07,
	};
	static constexpr uint8_t Output = 0;
	static constexpr uint8_t Input = 1;
};

const GPIO_expander_conf gpio_expander_conf = {
	.addr = 0b0100000, // un-shifted address
	.int_pin = {GPIO::D, 11, PinMode::Input},
	.IRQn = EXTI15_10_IRQn,
};
} // namespace mdrivlib
