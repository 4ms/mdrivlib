#pragma once
#include "pin.hh"
#include "system.hh"
#include <functional>

struct PinChangeConfig {
	uint32_t pin = 0;
	GPIO port = GPIO::A;
	bool on_rising_edge = false;
	bool on_falling_edge = false;
	uint32_t priority1 = 3;
	uint32_t priority2 = 3;
};

class PinChangeInterrupt {
public:
	PinChangeInterrupt();
	PinChangeInterrupt(const PinChangeConfig &config, std::function<void(void)> &&func);
	void init(const PinChangeConfig &config, std::function<void(void)> &&func);

	void start();
	void stop();

private:
	uint32_t _pin;
	void _init(const PinChangeConfig &config);
	std::function<void(void)> task_func;
};
