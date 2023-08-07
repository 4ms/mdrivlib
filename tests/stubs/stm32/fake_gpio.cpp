#include "fake_gpio.h"
#include <cstdio>

static bool FAKE_PINS[11][16] = {{0}};

void set_fake_pin(FakeGPIO port, uint16_t pin_num, bool newstate) {
	auto portnum = static_cast<uint8_t>(port);
	// printf("Writing port %d pin %d = %d\n", portnum, pin_num, newstate);
	FAKE_PINS[portnum][pin_num] = newstate;
}

bool read_fake_pin(FakeGPIO port, uint16_t pin_num) {
	auto portnum = static_cast<uint8_t>(port);
	// printf("Reading port %d pin %d = %d\n", portnum, pin_num, FAKE_PINS[portnum][pin_num]);
	return FAKE_PINS[portnum][pin_num];
}
