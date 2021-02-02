#pragma once

struct GPIO_expander_conf {
	uint32_t addr;
	Pin int_pin;
	IRQType IRQn;
};

template<unsigned NumGPIOs>
struct GPIOExpander {
	GPIOExpander(const GPIO_expander_conf &conf)
		: _conf{conf} {
	}

	void start() {
		// Create an EXTI interrupt to respond to chip telling us there's a pin change
		InterruptManager::registerISR(_conf.IRQn, [this]() { read_pins(); });
	}
	void read_pins() {
		// queue an I2C read:
		// i2c_.queue_read(conf.dev_addr, conf.reg_addr, num_bits, [this](uint8_t *received_data){
		//  for (int i=0; i<NumGPIOs; i++)
		// 		last_readings[i] = (received_data[i / 8] & (1<<(i & 0b111)))
		// 	});
	}

	bool last_readings[NumGPIOs];

private:
	const GPIO_expander_conf &_conf;
};

const GPIO_expander_conf gpio_expander_conf = {
	.addr = 0x00,
	.int_pin = {GPIO::D, 11, PinMode::Input},
	.IRQn = EXTI15_10_IRQn,
};

