#pragma once
#include <stdint.h>

enum LL_GPIO_STUB_VALS {
	LL_GPIO_MODE_INPUT,
	LL_GPIO_MODE_OUTPUT,
	LL_GPIO_MODE_ANALOG,
	LL_GPIO_MODE_ALTERNATE,
	LL_GPIO_PULL_UP,
	LL_GPIO_PULL_DOWN,
	LL_GPIO_PULL_NO,
	LL_GPIO_SPEED_FREQ_LOW,
	LL_GPIO_SPEED_FREQ_MEDIUM,
	LL_GPIO_SPEED_FREQ_HIGH,
	LL_GPIO_SPEED_FREQ_VERY_HIGH,
	LL_GPIO_OUTPUT_PUSHPULL,
	LL_GPIO_OUTPUT_OPENDRAIN,
};

enum class FakeGPIO : uint8_t {
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
};
bool read_fake_pin(FakeGPIO port, uint16_t pin_num);
void set_fake_pin(FakeGPIO port, uint16_t pin_num, bool newstate);
